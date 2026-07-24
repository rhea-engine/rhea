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
			parseIniFile(name, app, req, handler, buf, buflen);
			delete buf;
			});
		rs->onError([buf, handler](int err) {
			delete buf;
			handler(ReqError::DATA_INVALID, nullptr);
			});
		rs->ReadAsync(buf);
	}

	void SceneLoader::parseIniFile(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler, psio::IOBuffer* buf, size_t buflen)
	{
		std::unique_ptr<simpleparser::IniFileParser> parser = std::make_unique<simpleparser::IniFileParser>();
		std::vector<simpleparser::IniFileParser::Section> sections;
		UnitTree* tree = app->getUnitTree();
		std::string scene_root = "//" + req->getRealmName() + req->getParentPath();

		int entries = parser->parse(buf->reader(), buflen, sections);

		// initialize the scene
		std::string type_name = "//meworla/scene";
		if (sections.size() > 0 && sections[0].section_name == "scene") {
			if (sections[0].values["type"].length()) {
				type_name = sections[0].values["type"].toString();
			}
		}
		Unit* scene = tree->createUnit(name, type_name);
		scene->setOrigin(Component::ComponentOrigin::Configured);
		if (sections.size() > 0 && sections[0].section_name == "scene") {
			if (sections[0].values["name"].length())
				scene->setName(sections[0].values["name"].toString());
			scene->setup(app, sections[0].attributes, sections[0].values);
		}
		std::string basepath = "//" + req->getRealmName() + req->getParentPath() + "/" + scene->getPathName();

		// get all resources packed in this file to prevent reloading them
		std::vector<std::string> packed_resources;
		for (std::vector<simpleparser::IniFileParser::Section>::iterator it = sections.begin(); it != sections.end(); ++it) {
			simpleparser::IniFileParser::Section section = *it;
			packed_resources.push_back(section.section_name);
		}

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
		for (std::vector<simpleparser::IniFileParser::Section>::iterator it = sections.begin(); it != sections.end(); ++it) {
			simpleparser::IniFileParser::Section section = *it;
			if (section.section_name == "scene")
				continue;

			auto val = section.values.find("type");
			std::string type_name;
			if (val != section.values.end()) {
				type_name = val->second.toString();
			}
			else {
				LOG(LOG_ERROR) << "Type name not set";
				continue;
			}

			// split path into basename and path
			std::string path = section.section_name;
			size_t rpos = path.find_last_of("/");
			if (rpos == path.length()) {
				// trailing delim
				rpos = path.find_last_of("/", 1);
			}
			std::string ppath;
			Component* component;
			Unit* child_parent;
			// add basepath to attributes
			section.attributes["basepath"] = basepath;
			if (rpos != std::string::npos) {
				// create the unit and set config params
				ppath = path.substr(0, rpos);
				path = path.substr(rpos + 1);
				//unit = tree->createUnit(path, type_name, section.attributes, section.values);
				component = doCast<Component>(simpleparser::Type::create(path, type_name));
				component->setOrigin(Component::ComponentOrigin::Configured);

				// find parent relative to top parent			
				child_parent = doCast<Unit>(scene->find(ppath, true));
			}
			else {
				// create the unit and set config params
				//unit = tree->createUnit(path, type_name, section.attributes, section.values);
				component = doCast<Component>(simpleparser::Type::create(path, type_name));

				// find parent relative to top parent			
				child_parent = scene;
			}
			if (!component) {
				LOG(LOG_ERROR) << "Failed to create component of type " << type_name;
				continue;
			}
			child_parent->addChild(component);

			// store config params
			PendingUnitConfig config;
			config.component = component;
			config.parent = child_parent;
			config.section_name = section.section_name;
			config.attributes = section.attributes;
			config.values = section.values;
			pending_configs.push_back(config);
		}

		for (std::vector<PendingUnitConfig>::iterator it = pending_configs.begin(); it != pending_configs.end(); ++it) {
			PendingUnitConfig config = *it;

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
