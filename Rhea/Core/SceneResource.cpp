/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <memory>
#include <unordered_map>

#include <Psio/Log.h>
#include <SimpleParser/Type.h>

#include <Rhea/Application.h>
#include <Rhea/Core/SceneResource.h>
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/Resources/ResourceService.h>
#include <Rhea/UnitTree.h>

namespace rhea
{
	namespace
	{
		struct PendingUnitConfig
		{
			Component* component = nullptr;
			std::string section_name;
			std::unordered_map<std::string, std::string> attributes;
			std::unordered_map<std::string, simpleparser::Variant> values;
		};
	}

	SceneResource::~SceneResource()
	{
		if (m_prepared_prototype) {
			m_prepared_prototype->destroy();
			m_prepared_prototype = nullptr;
		}
	}

	void SceneResource::spawn(Unit* parent, const std::string& name, SpawnHandler handler)
	{
		if (!parent) {
			if (handler)
				handler(ReqError::PATH_NOT_FOUND, nullptr);
			return;
		}

		if (m_prepare_state == PrepareState::Prepared) {
			ReqError error;
			Unit* instance = spawnPrepared(parent, name, &error);

			if (handler)
				handler(error, instance);

			return;
		}

		InstanceRequest request;
		request.name = name;
		const std::string root_name = request.name.empty() ? m_definition.root.name : request.name;
		request.basepath = parent->getPath() + "/" + root_name;

		buildInstance(request, [parent, handler](ReqError error, Unit* root) {
			if (error != ReqError::OK) {
				if (handler)
					handler(error, nullptr);
				return;
			}

			parent->addChild(root);
			root->ready();
			if (handler)
				handler(ReqError::OK, root);
			});
	}

	void SceneResource::ensureMaterialized(MaterializeHandler handler)
	{
		if (m_materialized_instance) {
			if (handler)
				handler(ReqError::OK, m_materialized_instance);
			return;
		}

		if (handler)
			m_waiting_materialize_handlers.push_back(std::move(handler));

		if (m_materializing)
			return;

		m_materializing = true;
		InstanceRequest request;
		const std::string root_name = request.name.empty() ? m_definition.root.name : request.name;
		request.basepath = m_scene_root + "/" + root_name;

		buildInstance(request, [this](ReqError error, Unit* root) {
			if (error != ReqError::OK) {
				completeMaterialization(error, nullptr);
				return;
			}

			Unit* canonical_parent = doCast<Unit>(m_app->getUnitTree()->find(m_scene_root));
			if (!canonical_parent) {
				root->destroy();
				completeMaterialization(ReqError::PATH_NOT_FOUND, nullptr);
				return;
			}

			canonical_parent->addChild(root);
			root->ready();
			completeMaterialization(ReqError::OK, root);
			});
	}

	void SceneResource::completeMaterialization(ReqError error, Unit* instance)
	{
		m_materializing = false;
		if (error == ReqError::OK)
			m_materialized_instance = instance;

		auto handlers = std::move(m_waiting_materialize_handlers);
		m_waiting_materialize_handlers.clear();
		for (auto& handler : handlers) {
			if (handler)
				handler(error, instance);
		}
	}

	void SceneResource::buildInstance(const InstanceRequest& request, InstanceHandler handler)
	{
		std::unordered_map<std::string, Component*> instances;
		std::vector<PendingUnitConfig> pending_configs;
		std::vector<PendingSceneInstance> pending_instances;
		ResourceList resources(m_app);

		const std::string root_name = request.name.empty() ? m_definition.root.name : request.name;
		Unit* root = doCast<Unit>(simpleparser::Type::create(root_name, m_definition.root.type_name));
		if (!root) {
			if (handler)
				handler(ReqError::DATA_INVALID, nullptr);
			return;
		}

		root->setOrigin(Component::ComponentOrigin::Configured);
		auto rootAttributes = m_definition.root.attributes;
		rootAttributes["basepath"] = request.basepath;
		root->setup(m_app, rootAttributes, m_definition.root.values);
		instances[""] = root;
		instances[m_definition.root.name] = root;

		// Create every configured component before establishing relationships.
		for (const auto& node : m_definition.nodes) {
			Component* component = doCast<Component>(simpleparser::Type::create(node.name, node.type_name));
			if (!component) {
				LOG(LOG_ERROR) << "Failed to create component of type " << node.type_name;
				root->destroy();
				if (handler)
					handler(ReqError::DATA_INVALID, nullptr);
				return;
			}

			component->setOrigin(Component::ComponentOrigin::Configured);
			instances[node.node_id] = component;
			auto attributes = node.attributes;
			attributes["basepath"] = request.basepath;
			pending_configs.push_back({ component, node.node_id, attributes, node.values });
		}

		// Attach the configured tree. All referenced parents must be Units.
		for (const auto& node : m_definition.nodes) {
			auto component_it = instances.find(node.node_id);
			auto parent_it = instances.find(node.parent_id);
			Unit* definition_parent = parent_it == instances.end() ? nullptr : doCast<Unit>(parent_it->second);
			if (!definition_parent || component_it == instances.end()) {
				LOG(LOG_ERROR) << "Failed to find parent for component " << node.node_id;
				root->destroy();
				if (handler)
					handler(ReqError::DATA_INVALID, nullptr);
				return;
			}
			definition_parent->addChild(component_it->second);
		}

		auto collect_references = [this, &resources, &pending_instances](
			const std::string& owner_id, Component* owner) {
				for (const auto& reference : owner->getReferences()) {
					const std::string id = reference.value.toString();
					if (id.size() <= 1 || id[0] != '/')
						continue;

					const std::string resolved_id = id[1] == '/' ? id : m_scene_root + id;
					const std::string key = owner_id + "_" + reference.name + "_" + resolved_id;
					resources.push(key, resolved_id, reference.type);
					if (reference.usage != Component::ResourceUsage::Load) {
						pending_instances.push_back({
							key,
							doCast<Unit>(owner),
							"",
							reference.usage
							});
					}
				}
			};

		collect_references("scene", root);
		for (const auto& config : pending_configs) {
			config.component->setup(m_app, config.attributes, config.values);
			collect_references(config.section_name, config.component);
		}

		auto* resource_service = static_cast<ResourceService*>(m_app->getService("resources"));
		if (!resource_service) {
			root->destroy();
			if (handler)
				handler(ReqError::DATA_INVALID, nullptr);
			return;
		}

		resource_service->loadAsync(resources,
			[this, root, pending = std::move(pending_instances), handler](ResourceList* loaded, Error* error) mutable {
				if (error) {
					root->destroy();
					if (handler)
						handler(ReqError::DATA_INVALID, nullptr);
					return;
				}

				instantiatePendingScenes(*loaded, std::move(pending),
					[root, handler](ReqError result) {
						if (result != ReqError::OK) {
							root->destroy();
							if (handler)
								handler(result, nullptr);
							return;
						}
						if (handler)
							handler(ReqError::OK, root);
					});
			});
	}

	void SceneResource::instantiatePendingScenes(
		ResourceList& resources,
		std::vector<PendingSceneInstance> pending,
		CompletionHandler complete)
	{
		if (pending.empty()) {
			complete(ReqError::OK);
			return;
		}

		struct PendingState
		{
			size_t remaining = 0;
			bool completed = false;
			CompletionHandler complete;

			void finishOne(ReqError error)
			{
				if (completed)
					return;
				if (error != ReqError::OK) {
					completed = true;
					complete(error);
					return;
				}
				if (--remaining == 0) {
					completed = true;
					complete(ReqError::OK);
				}
			}
		};

		auto state = std::make_shared<PendingState>();
		state->remaining = pending.size();
		state->complete = std::move(complete);

		for (const auto& item : pending) {
			auto* resource = doCast<Resource>(resources[item.resource_key]);
			if (!resource) {
				state->finishOne(ReqError::DATA_INVALID);
				continue;
			}

			switch (item.usage) {
			case Component::ResourceUsage::Load:
				state->finishOne(ReqError::OK);
				break;
			case Component::ResourceUsage::Prepare:
				resource->prepare([state](ReqError error) {
					state->finishOne(error);
					});
				break;
			case Component::ResourceUsage::EnsureReady:
				resource->ensureReady([state](ReqError error, Unit*) {
					state->finishOne(error);
					});
				break;

			case Component::ResourceUsage::Instantiate: {
				if (!item.parent) {
					state->finishOne(ReqError::DATA_INVALID);
					break;
				}
				InstanceRequest request;
				request.name = item.instance_name;
				if (request.name.empty() && doCast<SceneResource>(resource))
					request.name = doCast<SceneResource>(resource)->getDefinition().root.name;
				request.basepath = item.parent->getPath() + "/" + request.name;
				resource->buildInstance(request,
					[state, parent = item.parent](ReqError error, Unit* instance) {
						if (error == ReqError::OK && instance)
							parent->addChild(instance);
						state->finishOne(error);
					});
				break;
			}
			}
		}
	}

	void SceneResource::prepare(PrepareHandler handler)
	{
		if (m_prepare_state == PrepareState::Prepared) {
			if (handler)
				handler(ReqError::OK);
			return;
		}

		if (handler)
			m_prepare_handlers.push_back(std::move(handler));

		if (m_prepare_state == PrepareState::Preparing) {
			return;
		}

		m_prepare_state = PrepareState::Preparing;

		InstanceRequest request;
		request.name = "";

		buildInstance(request, [this](ReqError error, Unit* root) {
			if (error != ReqError::OK) {
				if (root)
					root->destroy();
				m_prepare_state = PrepareState::Failed;
				m_prepared = false;
			}
			else {
				m_prepared_prototype = root;
				m_prepare_state = PrepareState::Prepared;
				m_prepared = true;
			}
			for (auto& h : m_prepare_handlers) {
				h(error);
			}
			m_prepare_handlers.clear();
			});
	}

	Unit* SceneResource::spawnPrepared(
		Unit* parent,
		const std::string& name,
		ReqError* error)
	{
		if (m_prepare_state != PrepareState::Prepared || !m_prepared_prototype) {
			if (error)
				*error = ReqError::IO_PENDING;
			return nullptr;
		}

		if (!parent) {
			if (error)
				*error = ReqError::PATH_NOT_FOUND;
			return nullptr;
		}

		const std::string instance_name =
			name.empty()
			? m_definition.root.name
			: name;
		Unit* instance = doCast<Unit>(m_prepared_prototype->instance(instance_name));

		if (!instance) {
			if (error)
				*error = ReqError::DATA_INVALID;
			return nullptr;
		}

		parent->addChild(instance);
		instance->ready();

		if (error)
			*error = ReqError::OK;

		return instance;
	}
}
