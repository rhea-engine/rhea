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
	void SceneLoader::load(std::string name, Application* app, orion::ResourceRequest* req, ResourceContext& context, Resource::LoadedHandler handler)
	{
		psio::Stream* rs = req->getStream();
		psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
		std::string res_id = "//" + req->getRealmName() + req->getPath();
		rs->onEnd([req, handler, buf, app, name, context, res_id](size_t buflen) {
			SceneResource::SceneDefinition scene_def;
			Error err = parseIniFile(name, buf->reader(), buflen, scene_def);
			if (err.isError()) {
				err.setResourceId(res_id);
				if (handler)
					handler(err, nullptr);
				delete buf;
				return;
			}
			SceneResource* scene_res = new SceneResource(name);
			if (scene_res) {
				scene_res->setup(app);
				const std::string source_resource_id = "//" + req->getRealmName() + req->getPath();
				scene_res->setDefinition(scene_def, context.source_directory, context.context_root, source_resource_id);
				handler(rhea::Error(), scene_res);
			}
			delete buf;
			});
		rs->onError([buf, handler, res_id](int err) {
			delete buf;
			Error error(Error::Code::RESOURCE_ERROR, "Could not read scene resource.", res_id);
			error.addContext("stream_error", std::to_string(err));

			if (handler)
				handler(error, nullptr);
			});
		rs->ReadAsync(buf);
	}

	Error SceneLoader::parseIniFile(std::string name, char* buf, size_t buflen, SceneResource::SceneDefinition &definition)
	{
		std::unique_ptr<simpleparser::IniFileParser> parser = std::make_unique<simpleparser::IniFileParser>();
		std::vector<simpleparser::IniFileParser::Section> sections;

		simpleparser::IniFileParser::Error err = parser->parse(buf, buflen, sections);

		if (err != simpleparser::IniFileParser::Error::PARSE_OK) {
			LOG(LOG_ERROR) << "Failed to parse scene file: " << name;
			return Error(Error::Code::INVALID_DATA, "Failed to parse scene file: " + name, name);
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
	
		Error result;
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
				if (!result.isError()) {
					Error type_err(Error::Code::INVALID_DATA, "Type name not set for resource: " + section.section_name, section.section_name);
					type_err.addContext("node", section.section_name);
					type_err.addContext("property", "type");
					result = type_err;
				}
				continue;
			}
			node.type_name = section.values["type"].toString();
			node.values = section.values;

			scene_def.nodes.push_back(node);
		}

		if (result.isError()) {
			return result;
		}
		definition = scene_def;
		return Error();
	}

	
}
