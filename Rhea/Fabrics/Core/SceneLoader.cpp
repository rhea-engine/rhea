#include <memory>

#include "UnitTree.h"
#include "SceneLoader.h"
#include "Application.h"
#include "SimpleParser/IniFileParser.h"
#include "Orion/ResourceStream.h"
#include "Scene.h"
#include <Resources/ResourceList.h>
#include <Resources/ResourceService.h>
#include <Psio/Log.h>

namespace Rhea
{	
	void SceneLoader::load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		std::unique_ptr<SimpleParser::IniFileParser> parser = std::make_unique<SimpleParser::IniFileParser>();
		std::vector<SimpleParser::IniFileParser::Section> sections;
		UnitTree* tree = app->getUnitTree();

		std::string scene_root = "//" + req->getRealmName() + req->getParentPath();

		Psio::Stream* rs = req->getStream();
		Psio::IOBuffer* buf = new Psio::IOBuffer(64 * 1024);
		rs->onEnd([&](size_t buflen) {

			int entries = parser->parse(buf->reader(), buflen, sections);

			// initialize the scene
			Scene* scene = static_cast<Scene*>(tree->createUnit(name, "//meworla/scene"));
			if (sections.size() > 0 && sections[0].section_name == "scene") {
				if (sections[0].values["name"].length())
					scene->setName(sections[0].values["name"]);
				scene->setup(app, sections[0].attributes, sections[0].values);
			}
			std::string basepath = "//" + req->getRealmName() + req->getParentPath() + "/" + scene->getPathName();

			// get all resources packed in this file to prevent reloading them
			std::vector<std::string> packed_resources;
			for (std::vector<SimpleParser::IniFileParser::Section>::iterator it = sections.begin(); it != sections.end(); ++it) {
				SimpleParser::IniFileParser::Section section = *it;
				packed_resources.push_back(section.section_name);
			}

			ResourceList res_list(app);

			// setup all resources
			for (std::vector<SimpleParser::IniFileParser::Section>::iterator it = sections.begin(); it != sections.end(); ++it) {
				SimpleParser::IniFileParser::Section section = *it;
				if (section.section_name == "scene")
					continue;

				auto val = section.values.find("type");
				std::string type_name;
				if (val != section.values.end()) {
					type_name = val->second;
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
				Unit* unit;
				Unit* child_parent;
				// add basepath to attributes
				section.attributes["basepath"] = basepath;
				if (rpos != std::string::npos) {
					// create the unit and set config params
					ppath = path.substr(0, rpos);
					path = path.substr(rpos + 1);
					unit = tree->createUnit(path, type_name, section.attributes, section.values);

					// find parent relative to top parent			
					child_parent = scene->find(ppath, true);
				}
				else {
					// create the unit and set config params
					unit = tree->createUnit(path, type_name, section.attributes, section.values);

					// find parent relative to top parent			
					child_parent = scene;
				}
				child_parent->addChild(unit);

				static int i = 0;
				i++;

				// resource to load?
				std::vector<Unit::Export> references = unit->getReferences();
				for (auto ref_it = references.begin(); ref_it != references.end(); ++ref_it) {
					std::string id = ref_it->value;
					// if absolute path or base path it is a resource not satified by this scene file
					if (id.size() > 1 && id[0] == '/') {
						if (id[1] == '/')
							res_list.push(section.section_name + '_' + id, id, ref_it->type);
						else
							res_list.push(section.section_name + '_' + id, scene_root + id, ref_it->type);
					}
				}
			}
			delete buf;

			// get dependencies
			LOG(LOG_INFO) << "scene loaded. Loading dependencies.";
			static_cast<ResourceService*>(app->getService("resources"))->loadAsync(res_list, [scene, handler](ResourceList* rh, Error* err) {
				handler(ReqError::OK, scene);
			});

			});
		rs->onError([buf, handler](int err) {
			delete buf;
			handler(ReqError::DATA_INVALID, nullptr);
			});
		rs->ReadAsync(buf);
	}
}
