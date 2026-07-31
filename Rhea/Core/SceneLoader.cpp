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
#include <Rhea/Core/SceneResource.h>

namespace rhea
{	
	void SceneLoader::load(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		psio::Stream* rs = req->getStream();
		psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
		rs->onEnd([req, handler, buf, app, name](size_t buflen) {
			std::string scene_root = "//" + req->getRealmName() + req->getParentPath();
			SceneResource::SceneDefinition scene_def = parseIniFile(name, scene_root, buf->reader(), buflen);
			SceneResource* scene_res = new SceneResource(name);
			if (scene_res) {
				scene_res->setup(app);
				scene_res->setDefinition(scene_def, scene_root);
				handler(ReqError::OK, scene_res);
			}
			delete buf;
			});
		rs->onError([buf, handler](int err) {
			delete buf;
			handler(ReqError::DATA_INVALID, nullptr);
			});
		rs->ReadAsync(buf);
	}

	SceneResource::SceneDefinition SceneLoader::parseIniFile(std::string name, const std::string& scene_root, char* buf, size_t buflen)
	{
		std::unique_ptr<simpleparser::IniFileParser> parser = std::make_unique<simpleparser::IniFileParser>();
		std::vector<simpleparser::IniFileParser::Section> sections;

		int entries = parser->parse(buf, buflen, sections);

		if (!entries) {
			LOG(LOG_ERROR) << "Failed to parse scene file: " << name;
			return SceneResource::SceneDefinition();
		}		
		SceneResource::SceneDefinition scene_def;

		// initialize the scene
		scene_def.root.name = name;
		std::string type_name = "//meworla/scene";
		if (sections.size() > 0 && sections[0].section_name == "scene") {
			if (sections[0].values["type"].length()) {
				type_name = sections[0].values["type"].toString();
			}
		}
		scene_def.root.type_name = type_name;
		if (sections.size() > 0 && sections[0].section_name == "scene") {
			if (sections[0].values.find("name") != sections[0].values.end() && sections[0].values["name"].length())
				scene_def.root.name = sections[0].values["name"].toString();
			scene_def.root.attributes = sections[0].attributes;
			scene_def.root.values = sections[0].values;
		}
		
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

			SceneResource::NodeDefinition node;
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

			scene_def.nodes.push_back(node);
		}

		return scene_def;
	}

	
}
