/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <memory>

#include "Rhea/UnitTree.h"
#include "Rhea/Core/SceneLoader.h"
#include "Rhea/Application.h"
#include "SimpleParser/IniFileParser.h"
#include "Orion/ResourceStream.h"
#include "Rhea/Core/Scene.h"
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/Resources/ResourceService.h>
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

	void SceneLoader::load(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		psio::Stream* rs = req->getStream();
		psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
		rs->onEnd([req, handler, buf, app, name](size_t buflen) {
			std::string scene_root = "//" + req->getRealmName() + req->getParentPath();
			SceneDefinition* scene_def = parseIniFile(name, scene_root, buf->reader(), buflen);
			if (scene_def)
				instantiate(app, handler, scene_root, scene_def); // legacy, but we need to instantiate the scene to get the dependencies
			delete buf;
			delete scene_def;
			});
		rs->onError([buf, handler](int err) {
			delete buf;
			handler(ReqError::DATA_INVALID, nullptr);
			});
		rs->ReadAsync(buf);
	}

	SceneLoader::SceneDefinition* SceneLoader::parseIniFile(std::string name, const std::string& scene_root, char* buf, size_t buflen)
	{
		std::unique_ptr<simpleparser::IniFileParser> parser = std::make_unique<simpleparser::IniFileParser>();
		std::vector<simpleparser::IniFileParser::Section> sections;

		int entries = parser->parse(buf, buflen, sections);

		if (!entries) {
			LOG(LOG_ERROR) << "Failed to parse scene file: " << name;
			return nullptr;
		}

		SceneDefinition* scene_def = new SceneDefinition();

		// initialize the scene
		scene_def->root.name = name;
		std::string type_name = "//meworla/scene";
		if (sections.size() > 0 && sections[0].section_name == "scene") {
			if (sections[0].values["type"].length()) {
				type_name = sections[0].values["type"].toString();
			}
		}
		scene_def->root.type_name = type_name;
		if (sections.size() > 0 && sections[0].section_name == "scene") {
			if (sections[0].values.find("name") != sections[0].values.end() && sections[0].values["name"].length())
				scene_def->root.name = sections[0].values["name"].toString();
			scene_def->root.attributes = sections[0].attributes;
			scene_def->root.values = sections[0].values;
		}
		std::string basepath = scene_root + "/" + scene_def->root.name;

		// get all resources packed in this file 
	
		for (auto section : sections) {
			if (section.section_name == "scene")
				continue;

			// split path into basename and path
			std::string path = section.section_name;
			size_t rpos = path.find_last_of("/");
			if (rpos == path.length()) {
				// one trailing delim
				rpos = path.find_last_of("/", 1);
			}
			std::string ppath;
			if (rpos != std::string::npos) {
				// create the unit and set config params
				ppath = path.substr(0, rpos);
				path = path.substr(rpos + 1);
			}
			// add basepath to attributes
			section.attributes["basepath"] = basepath;

			NodeDefinition node;
			node.node_id = section.section_name;
			node.parent_id = ppath;
			node.name = path;
			node.attributes = section.attributes;
			if (section.values.find("type") == section.values.end()) {
				LOG(LOG_ERROR) << "Type name not set for resource: " << section.section_name;
				continue;
			}
			node.type_name = section.values["type"].toString();
			node.values = section.values;

			scene_def->nodes.push_back(node);
		}

		return scene_def;
	}

	void SceneLoader::instantiate(Application* app, Resource::LoadedHandler handler, const std::string& scene_root, SceneDefinition* scene_def)
	{
		UnitTree* tree = app->getUnitTree();

		std::unordered_map<std::string, Component*> instances;

		// initialize the scene
		Unit* scene = tree->createUnit(scene_def->root.name, scene_def->root.type_name);
		scene->setOrigin(Component::ComponentOrigin::Configured);
		scene->setup(app, scene_def->root.attributes, scene_def->root.values);
		
		instances[""] = scene;

		ResourceList res_list(app);

		// add here because scene is skipped below
		std::vector<Unit::Export> references = scene->getReferences();
		for (auto ref_it = references.begin(); ref_it != references.end(); ++ref_it) {
			std::string id = ref_it->value.toString();
			// if absolute path or base path it is a resource not satified by this scene file
			if (id.size() > 1 && id[0] == '/') {
				if (id[1] == '/')
					res_list.push("scene_" + id, id, ref_it->type);
				else
					res_list.push("scene_" + id, scene_root + id, ref_it->type);
			}
		}

		std::vector<PendingUnitConfig> pending_configs;

		// instantiate all resources
		for (const auto& node : scene_def->nodes) {
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

		for (const auto& node : scene_def->nodes) {
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
			component->setup(app, config.attributes, config.values);

			static int i = 0;
			i++;

			// resource to load?
			std::vector<Component::Export> references = component->getReferences();
			for (auto ref_it = references.begin(); ref_it != references.end(); ++ref_it) {
				std::string id = ref_it->value.toString();
				// if absolute path or base path it is a resource not satified by this scene file
				if (id.size() > 1 && id[0] == '/') {
					if (id[1] == '/')
						res_list.push(config.section_name + '_' + id, id, ref_it->type);
					else
						res_list.push(config.section_name + '_' + id, scene_root + id, ref_it->type);
				}
			}
		}

		// get dependencies
		LOG(LOG_INFO) << "scene loaded. Loading dependencies.";
		static_cast<ResourceService*>(app->getService("resources"))->loadAsync(res_list, [scene, handler](ResourceList* rh, Error* err) {
			handler(ReqError::OK, scene);
			});
	}
}
