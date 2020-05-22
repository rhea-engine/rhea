#include <memory>

#include "UnitTree.h"
#include "SceneLoader.h"
#include "Application.h"
#include "SimpleParser/IniFileParser.h"
#include "Orion/Resource.h"
#include "Orion/ResourceStream.h"
#include "Scene.h"

namespace Rhea
{	
	Scene* SceneLoader::load(Unit* parent, Orion::Resource* resource, Application* app)
	{
		std::unique_ptr<SimpleParser::IniFileParser> parser = std::make_unique<SimpleParser::IniFileParser>();
		std::vector<SimpleParser::IniFileParser::Section> sections;
		UnitTree* tree = app->getUnitTree();

		Orion::ResourceStream* rs = resource->Open(1);
		char* buf;
		size_t buflen;
		buflen = rs->ReadBuffer(&buf);
		if (buflen <= 0)
			return NULL;
		delete rs;

		int entries = parser->parse(buf, buflen, sections);
		// create the scene
		Scene* scene;
		if (sections.size() > 0 && sections[0].section_name == "scene") {
			if (sections[0].values["name"].length())
				scene = new Scene(sections[0].values["name"]);
			else
				scene = new Scene(resource->get_name());
		}
		else
			scene = new Scene(resource->get_name());
		scene->setup(app);
		parent->addChild(scene);

		// get all resources packed in this file to prevent reloading them
		std::vector<std::string> packed_resources;
		for (std::vector<SimpleParser::IniFileParser::Section>::iterator it = sections.begin(); it != sections.end(); ++it) {
			SimpleParser::IniFileParser::Section section = *it; 
			packed_resources.push_back(section.section_name);
		}

		for (std::vector<SimpleParser::IniFileParser::Section>::iterator it = sections.begin(); it != sections.end(); ++it) {
			SimpleParser::IniFileParser::Section section = *it;
			std::string type_name = section.values["type"];

			if (section.section_name == "scene")
				continue;

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
			if (rpos != std::string::npos) {
				// create the unit and set config params
				unit = tree->createUnit(path.substr(rpos + 1), type_name, section.attributes, section.values);

				// find parent relative to top parent			
				child_parent = scene->find(path.substr(0, rpos), true);
			}
			else {
				// create the unit and set config params
				unit = tree->createUnit(path, type_name, section.attributes, section.values);

				// find parent relative to top parent			
				child_parent = scene;
			}
			child_parent->addChild(unit);				
			if ((type_name == "//meworla/item") || (type_name == "//meworla/camera")) {
				scene->addItem((Item*)unit);
			}
		}
		free(buf);

		return scene;		
	}
}
