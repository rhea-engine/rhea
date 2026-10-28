/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <memory>
#include <unordered_map>
#include <unordered_set>

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
			if (handler) {
				ErrorReport error;
				error.addError(Error(Error::Code::NOT_FOUND, "Parent unit not found.", m_source_resource_id));
				handler(error, nullptr);
			}
			return;
		}

		if (m_prepare_state == PrepareState::Prepared) {
			ErrorReport error;

			Unit* instance = spawnPrepared(parent, name, &error);

			if (handler)
				handler(error, instance);

			return;
		}

		InstanceRequest request;
		request.name = name;
		const std::string root_name = request.name.empty() ? m_definition.root.name : request.name;
		request.basepath = parent->getPath() + "/" + root_name;

		buildInstance(request, [this, parent, handler](ErrorReport error, Unit* root) {
			if (error.isError()) {
				if (handler)
					handler(error, nullptr);
				return;
			}

			if (!parent->addChild(root)) {
				root->destroy();
				if (handler) {
					error.addError(Error(Error::Code::INVALID_DATA, "Failed to add child unit.", m_source_resource_id));
					handler(error, nullptr);
				}
				return;
			}
			root->ready();
			if (handler)
				handler(error, root);
			});
	}

	void SceneResource::ensureMaterialized(MaterializeHandler handler)
	{
		if (m_materialized_instance) {
			if (handler)
				handler(ErrorReport(), m_materialized_instance);
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

		buildInstance(request, [this](ErrorReport error, Unit* root) {
			if (error.isError()) {
				completeMaterialization(error, nullptr);
				return;
			}

			Unit* canonical_parent = doCast<Unit>(m_app->getUnitTree()->find(m_scene_root));
			if (!canonical_parent) {
				root->destroy();
				error.addError(Error(Error::Code::NOT_FOUND, "Parent unit not found.", m_source_resource_id));
				completeMaterialization(error, nullptr);
				return;
			}

			if (!canonical_parent->addChild(root)) {
				root->destroy();
				error.addError(Error(Error::Code::INVALID_DATA, "Failed to add child unit.", m_source_resource_id));
				completeMaterialization(error, nullptr);
				return;
			}
			root->ready();
			completeMaterialization(error, root);
			});
	}

	void SceneResource::completeMaterialization(ErrorReport error, Unit* instance)
	{
		m_materializing = false;
		if (!error.isError())
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
		std::unordered_set<std::string> node_ids;
		for (const auto& node : m_definition.nodes) {
			if (!node_ids.insert(node.node_id).second) {
				LOG(LOG_ERROR) << "Duplicate scene component: " << node.node_id;
				ErrorReport error;
				error.addError(Error(Error::Code::INVALID_DATA, "Duplicate scene component: " + node.node_id, m_source_resource_id));
				error.errors.back().addContext("node", node.node_id);
				if (handler)
					handler(error, nullptr);
				return;
			}
		}

		std::unordered_map<std::string, Component*> instances;
		std::vector<PendingUnitConfig> pending_configs;
		std::vector<PendingSceneInstance> pending_instances;
		const ResourceContext context{ m_context_root, m_scene_root };
		ResourceList resources(m_app, context);
		std::vector<PendingSceneInstance> ready_instances;
		ResourceList ready_resources(m_app, context);

		// check types
		ErrorReport type_error;
		if (!simpleparser::Type::get(m_definition.root.type_name)) {
			Error error(Error::Code::UNKNOWN_TYPE, "Type is not registered: " + m_definition.root.type_name, m_source_resource_id);
			error.addContext("node", m_definition.root.name);
			error.addContext("type", m_definition.root.type_name);
			type_error.addError(error);
		}
		for (const auto& node : m_definition.nodes) {
			if (simpleparser::Type::get(node.type_name))
				continue;
			Error error(Error::Code::UNKNOWN_TYPE, "Type is not registered: " + node.type_name, m_source_resource_id);
			error.addContext("node", node.node_id);
			error.addContext("type", node.type_name);
			type_error.addError(error);
		}
		if (type_error.isError()) {
			if (handler)
				handler(std::move(type_error), nullptr);
			return;
		}

		const std::string root_name = request.name.empty() ? m_definition.root.name : request.name;
		Component* root_component = doCast<Component>(simpleparser::Type::create(root_name, m_definition.root.type_name));
		if (!root_component) {
			LOG(LOG_ERROR) << "Failed to create component of type " << m_definition.root.type_name;
			if (handler) {
				ErrorReport error;
				error.addError(Error(Error::Code::INVALID_DEFINITION, "Type is not registered: " + m_definition.root.type_name, m_source_resource_id));
				handler(error, nullptr);
			}
			return;
		}
		Unit* root = doCast<Unit>(root_component);
		if (!root) {
			delete root_component;
			LOG(LOG_ERROR) << "Root component is not a Unit: " << m_definition.root.name;
			if (handler) {
				ErrorReport error;
				error.addError(Error(Error::Code::INVALID_DATA, "Root component is not a Unit: " + m_definition.root.name, m_source_resource_id));
				handler(std::move(error), nullptr);
			}
			return;
		}

		root->setOrigin(Component::ComponentOrigin::Configured);
		auto rootAttributes = m_definition.root.attributes;
		rootAttributes["basepath"] = request.basepath;
		root->setup(m_app, rootAttributes, m_definition.root.values);
		instances[""] = root;
		instances[m_definition.root.name] = root;

		auto destroy_instances = [&]() {
			std::vector<Component*> detached_roots;

			for (const auto& config : pending_configs) {
				if (!config.component->getParent())
					detached_roots.push_back(config.component);
			}

			for (Component* component : detached_roots)
				component->destroy();

			root->destroy();
			};

		// Create every configured component before establishing relationships.
		ErrorReport create_error;
		for (const auto& node : m_definition.nodes) {
			Component* component = doCast<Component>(simpleparser::Type::create(node.name, node.type_name));
			if (!component) {
				LOG(LOG_ERROR) << "Failed to create component of type " << node.type_name;
				create_error.addError(Error(Error::Code::INVALID_DEFINITION, "Failed to instantiate type: " + node.type_name, m_source_resource_id));
				create_error.errors.back().addContext("node", node.node_id);
				create_error.errors.back().addContext("type", node.type_name);
				continue;
			}

			component->setOrigin(Component::ComponentOrigin::Configured);
			instances[node.node_id] = component;
			auto attributes = node.attributes;
			attributes["basepath"] = request.basepath;
			pending_configs.push_back({ component, node.node_id, attributes, node.values });
		}
		if (create_error.isError()) {
			destroy_instances();
			if (handler)
				handler(create_error, nullptr);
			return;
		}

		// Attach the configured tree. All referenced parents must be Units.
		ErrorReport attach_errors;
		for (const auto& node : m_definition.nodes) {
			auto component_it = instances.find(node.node_id);
			auto parent_it = instances.find(node.parent_id);
			Unit* definition_parent = parent_it == instances.end() ? nullptr : doCast<Unit>(parent_it->second);
			if (!definition_parent || component_it == instances.end()) {
				LOG(LOG_ERROR) << "Failed to find parent for component " << node.node_id;
				attach_errors.addError(Error(Error::Code::INVALID_DATA, "Failed to find parent for component " + node.node_id, m_source_resource_id));
				continue;
			}
			if (!definition_parent->addChild(component_it->second)) {
				LOG(LOG_ERROR) << "Failed to attach scene component " << node.node_id;
				attach_errors.addError(Error(Error::Code::INVALID_DATA, "Failed to attach scene component " + node.node_id, m_source_resource_id));
				continue;
			}
		}
		if (attach_errors.isError()) {
			destroy_instances();
			if (handler)
				handler(attach_errors, nullptr);
			return;
		}

		ErrorReport references_error;
		auto collect_references = [this, &request, &instances, &resources, &pending_instances, &ready_resources, &ready_instances, &references_error](
			const std::string& owner_id, Component* owner)->bool {
				for (const auto& reference : owner->getReferences()) {
					const std::string id = reference.value.toString();
					if (id.size() <= 1)
						continue;

					bool is_local = id[0] != '/';
					std::string resolved_id;
					Unit* target = nullptr;
					if (is_local) {
						// Local objects already exist; only additional lifecycle work
						// requires an entry in the resource list.
						if (reference.usage == Component::ResourceUsage::Load)
							// Local object references are instantiated when the tree is attached.
							continue;

						// Find the configured object in this instance.
						auto target_it = instances.find(id);
						target = target_it == instances.end() ? nullptr : doCast<Unit>(target_it->second);

						if (!target) {
							LOG(LOG_ERROR) << "Local resource reference not found: " << id << " for " << owner_id;
							Error reference_error = Error(Error::Code::INVALID_DATA, "Local resource reference not found: " + id, m_source_resource_id);
							reference_error.addContext("reference", id);
							reference_error.addContext("node", owner_id);
							reference_error.addContext("property", reference.name);
							references_error.addError(reference_error);
							return false;
						}

						resolved_id = request.basepath + "/" + id;
					}
					else {
						resolved_id = id[1] == '/' ? id : m_context_root + id;
						// Also recognize absolute references into this instance.
						const std::string local_prefix = request.basepath + "/";
						if (!request.basepath.empty() && resolved_id.compare(0, local_prefix.size(), local_prefix) == 0) {
							const auto target_it = instances.find(resolved_id.substr(local_prefix.size()));

							if (target_it != instances.end()) {
								target = doCast<Unit>(target_it->second);

								if (reference.usage == Component::ResourceUsage::Load)
									continue;

								if (!target) {
									Error reference_error = Error(Error::Code::INVALID_DATA, "Target is not a unit: " + id, m_source_resource_id);
									references_error.addError(reference_error);
									return false;
								}
							}
						}
							
					}
					const std::string key = owner_id + "_" + reference.name + "_" + resolved_id;
					const bool ensure_ready = reference.usage == Component::ResourceUsage::EnsureReady;
					auto& resource_list = ensure_ready ? ready_resources : resources;
					auto& pending_list = ensure_ready ? ready_instances : pending_instances;
					resource_list.push(key, resolved_id, reference.type, false, target);
					if (reference.usage != Component::ResourceUsage::Load) {
						pending_list.push_back({
							key,
							doCast<Unit>(owner),
							"",
							reference.usage
							});
					}
				}
				return true;
			};

		for (const auto& config : pending_configs)
			config.component->setup(m_app, config.attributes, config.values);
		
		size_t owner_index = 0;
		bool references_valid = !references_error.isError();
		root->traverse([&](std::string& name, Component* component)->bool {
			component->resolveReferences(m_context_root, m_scene_root);

			if (!references_valid)
				return false;

			const std::string owner_id = std::to_string(owner_index++) + "_" + name;
			references_valid = collect_references(owner_id, component);
			return references_valid;
			});

		if (!references_valid) {
			root->destroy();
			if (handler)
				handler(references_error, nullptr);
			return;
		}

		auto* resource_service = static_cast<ResourceService*>(m_app->getService("resources"));
		if (!resource_service) {
			root->destroy();
			if (handler) {
				ErrorReport error;
				error.addError(Error(Error::Code::INVALID_DATA, "Resource service not available.", m_source_resource_id));
				handler(std::move(error), nullptr);
			}
			return;
		}

		auto load_remaining = [this, root, resource_service, resources = std::move(resources), pending_instances, handler](ErrorReport report) mutable {
			resource_service->loadAsync(resources,
				[this, root, pending = std::move(pending_instances), handler, report](ResourceList* loaded, ErrorReport error) mutable {
					report.merge(std::move(error));
					if (report.isError()) {
						root->destroy();
						if (handler)
							handler(std::move(report), nullptr);
						return;
					}

					instantiatePendingScenes(*loaded, std::move(pending),
						[root, report, handler](ErrorReport result) mutable {
							report.merge(std::move(result));
							if (report.isError()) {
								root->destroy();
								if (handler)
									handler(std::move(report), nullptr);
								return;
							}
							if (handler)
								handler(std::move(report), root);
						});
				});
		};

		// Materialize libraries before resolving references to their configured nodes.
		resource_service->loadAsync(ready_resources,
			[this, root, pending = std::move(ready_instances), load_remaining, handler](ResourceList* loaded, ErrorReport error) mutable {
				if (error.isError()) {
					root->destroy();
					if (handler)
						handler(error, nullptr);
					return;
				}
				instantiatePendingScenes(*loaded, std::move(pending),
					[root, error, load_remaining, handler](ErrorReport result) mutable {
						error.merge(result);
						if (result.isError()) {
							root->destroy();
							if (handler)
								handler(error, nullptr);
							return;
						}
						load_remaining(std::move(error));
					});
			});
	}

	void SceneResource::instantiatePendingScenes(
		ResourceList& resources,
		std::vector<PendingSceneInstance> pending,
		CompletionHandler complete)
	{
		if (pending.empty()) {
			complete(ErrorReport());
			return;
		}

		struct PendingState
		{
			size_t remaining = 0;
			bool completed = false;
			CompletionHandler complete;
			ErrorReport report = ErrorReport();

			void finishOne(ErrorReport error)
			{
				if (completed)
					return;
				
				report.merge(std::move(error));

				if (--remaining == 0) {
					completed = true;
					complete(std::move(report));
				}
			}
		};

		auto state = std::make_shared<PendingState>();
		state->remaining = pending.size();
		state->complete = std::move(complete);

		for (const auto& item : pending) {
			auto* resource = doCast<Resource>(resources[item.resource_key]);
			if (!resource) {
				ErrorReport report;
				report.addError(Error(Error::Code::NOT_FOUND, "Resource not found: " + item.resource_key, m_source_resource_id));
				state->finishOne(std::move(report));
				continue;
			}

			switch (item.usage) {
			case Component::ResourceUsage::Load:
				state->finishOne(ErrorReport());
				break;
			case Component::ResourceUsage::Prepare:
				resource->prepare([state](ErrorReport error) {
					state->finishOne(error);
					});
				break;
			case Component::ResourceUsage::EnsureReady:
				resource->ensureReady([state](ErrorReport error, Unit*) {
					state->finishOne(error);
					});
				break;

			case Component::ResourceUsage::Instantiate: {
				if (!item.parent) {
					ErrorReport report;
					report.addError(Error(Error::Code::INVALID_DEFINITION, "Parent not found.", m_source_resource_id));
					state->finishOne(std::move(report));
					break;
				}
				InstanceRequest request;
				request.name = item.instance_name;
				if (request.name.empty() && doCast<SceneResource>(resource))
					request.name = doCast<SceneResource>(resource)->getDefinition().root.name;
				request.basepath = item.parent->getPath() + "/" + request.name;
				resource->buildInstance(request,
					[state, parent = item.parent, source_resource_id = m_source_resource_id](ErrorReport report, Unit* instance) {
						if (!report.isError()) {
							if (!instance) {
								report.addError(Error(Error::Code::INVALID_DATA, "Resource returned no instance.", source_resource_id));
							}
							else if (!parent->addChild(instance)) {
								instance->destroy();
								report.addError(Error(Error::Code::INVALID_DATA, "Failed to add child unit.", source_resource_id));
							}
						}
						state->finishOne(std::move(report));
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
				handler(ErrorReport());
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

		buildInstance(request, [this](ErrorReport error, Unit* root) {
			if (error.isError()) {
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
			auto handlers = std::move(m_prepare_handlers);
			for (auto& h : handlers) {
				h(error);
			}
			m_prepare_handlers.clear();
			});
	}

	Unit* SceneResource::spawnPrepared(
		Unit* parent,
		const std::string& name,
		ErrorReport* error)
	{
		if (error)
			*error = ErrorReport{};

		if (m_prepare_state != PrepareState::Prepared || !m_prepared_prototype) {
			if (error)
				error->addError(Error(Error::Code::UNAVAILABLE, "Instance is not prepared.", m_source_resource_id)); // PENDING?
			return nullptr;
		}

		if (!parent) {
			if (error)
				error->addError(Error(Error::Code::NOT_FOUND, "Parent not found.", m_source_resource_id));
			return nullptr;
		}

		const std::string instance_name =
			name.empty()
			? m_definition.root.name
			: name;
		Unit* instance = doCast<Unit>(m_prepared_prototype->instance(instance_name));

		if (!instance) {
			if (error)
				error->addError(Error(Error::Code::INVALID_DATA, "Instance not found.", m_source_resource_id));
			return nullptr;
		}

		if (!parent->addChild(instance)) {
			instance->destroy();
			if (error)
				error->addError(Error(Error::Code::INVALID_DATA, "Failed to add child unit.", m_source_resource_id));
			return nullptr;
		}
		instance->ready();

		return instance;
	}
}
