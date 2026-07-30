/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include <Rhea/Core/SceneResource.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <SimpleParser/IniFileParser.h>
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/Resources/ResourceService.h>
#include <Rhea/Resources/Resources.h>
#include <Psio/Log.h>

namespace rhea
{
	struct PendingUnitConfig
	{
		Component* component = nullptr;
		Unit* parent = nullptr;
		std::string section_name;

		simpleparser::IniFileParser::AttributeMap attributes;
		simpleparser::IniFileParser::ValueMap values;

		bool isSceneRoot = false;
	};

	void SceneResource::spawn(
		Unit* runtime_parent,
		SpawnHandler handler)
	{
		instantiate(nullptr, [runtime_parent, handler](ReqError error, Unit* root) {
			if (error != ReqError::OK) {
				handler(error, nullptr);
				return;
			}

			runtime_parent->addChild(root);
			root->ready();
			handler(ReqError::OK, root);
			});
	}

	void SceneResource::instantiate(rhea::Unit* definition_parent, SpawnHandler handler)
	{
		UnitTree* tree = m_app->getUnitTree();

		std::unordered_map<std::string, Component*> instances;

		// initialize the scene
		Unit* scene = tree->createUnit(m_definition.root.name, m_definition.root.type_name);
		scene->setOrigin(Component::ComponentOrigin::Configured);
		scene->setup(m_app, m_definition.root.attributes, m_definition.root.values);

		instances[""] = scene;

		ResourceList res_list(m_app);

		// add here because scene is skipped below
		std::vector<Unit::ResourceReference> references = scene->getReferences();
		for (auto ref_it = references.begin(); ref_it != references.end(); ++ref_it) {
			std::string id = ref_it->value.toString();
			// if absolute path or base path it is a resource not satified by this scene file
			if (id.size() > 1 && id[0] == '/') {
				if (id[1] == '/')
					res_list.push("scene_" + id, id, ref_it->type);
				else
					res_list.push("scene_" + id, m_scene_root + id, ref_it->type);
			}
		}

		std::vector<PendingUnitConfig> pending_configs;

		// instantiate all resources
		for (const auto& node : m_definition.nodes) {
			if (node.node_id == "scene")
				continue;

			auto val = node.values.find("type");
			std::string type_name;
			if (val != node.values.end()) {
				type_name = val->second.toString();
			}
			else {
				LOG(LOG_ERROR) << "Type name not set";
				continue;
			}

			// create the unit and set config params			
			Component* component = doCast<Component>(simpleparser::Type::create(node.name, type_name));
			if (!component) {
				LOG(LOG_ERROR) << "Failed to create component of type " << type_name;
				continue;
			}
			instances[node.node_id] = component;
			if (component)
				component->setOrigin(Component::ComponentOrigin::Configured);

			// store config params
			PendingUnitConfig config;
			config.component = component;
			config.section_name = node.node_id;
			config.attributes = node.attributes;
			config.values = node.values;
			pending_configs.push_back(config);
		}

		// add relationships

		for (const auto& node : m_definition.nodes) {
			Unit* parent = node.parent_id.empty()
				? scene
				: doCast<Unit>(instances.at(node.parent_id));
			if (parent)
				parent->addChild(instances.at(node.node_id));
			else {
				LOG(LOG_ERROR) << "Failed to find parent for component " << node.node_id;
				continue;
			}
		}

		for (const auto& config : pending_configs) {

			Component* component = config.component;

			// setup component with config params
			component->setup(m_app, config.attributes, config.values);

			static int i = 0;
			i++;

			// resource to load?
			std::vector<Component::ResourceReference> references = component->getReferences();
			bool instantiate = doCast<Resources>(component) != nullptr;
			for (auto ref_it = references.begin(); ref_it != references.end(); ++ref_it) {
				std::string id = ref_it->value.toString();
				// if absolute path or base path it is a resource not satified by this scene file
				if (id.size() > 1 && id[0] == '/') {
					if (id[1] == '/')
						res_list.push(config.section_name + '_' + id, id, ref_it->type);
					else
						res_list.push(config.section_name + '_' + id, m_scene_root + id, ref_it->type);
					if (instantiate)
						pending_instances.push_back({ config.section_name, doCast<Unit>(component), "", SceneReferenceMode::Materialize });
				}
			}
		}

		// get dependencies
		LOG(LOG_INFO) << "scene loaded. Loading dependencies.";
		static_cast<ResourceService*>(m_app->getService("resources"))->loadAsync(res_list, [this, &res_list, scene, definition_parent, handler](ResourceList* rh, Error* err) {
			instantiatePendingScenes(
				res_list,
				pending_instances,
				[scene, definition_parent, handler](ReqError error) {
					if (error != ReqError::OK) {
						scene->destroy();
						handler(ReqError::DATA_INVALID, nullptr);
						return;
					}

					if (definition_parent)
						definition_parent->addChild(scene);

					handler(ReqError::OK, scene);
				});
			});
	}

	void SceneResource::instantiatePendingScenes(ResourceList& resources, std::vector<PendingSceneInstance> pending, CompletionHandler complete)
	{
		if (pending.empty()) {
			complete(ReqError::OK);
			return;
		}

		struct PendingState
		{
			int remaining;
			CompletionHandler complete;
			void finishOne(ReqError error)
			{
				if (error != ReqError::OK) {
					complete(error);
					return;
				}
				remaining--;
				if (remaining <= 0) {
					complete(ReqError::OK);
				}
			}
		};

		auto state = std::make_shared<PendingState>();
		state->remaining = pending.size();
		state->complete = std::move(complete);

		for (const auto& item : pending) {
			auto* resource =
				doCast<SceneResource>(resources[item.resource_key]);

			if (!resource) {
				state->finishOne(ReqError::OK);
				continue;
			}

			resource->instantiate(
				item.parent,
				[state](ReqError error, Unit*) {
					state->finishOne(error);
				});
		}
	}
}