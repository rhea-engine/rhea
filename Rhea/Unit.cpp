#include "Unit.h"

namespace Rhea
{
	void Unit::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, std::string> values)
	{
		m_attributes = attributes;
		m_values = values;
	}

	void Unit::addChild(Unit* child)
	{		
		child->m_path = m_path + "/" + child->m_pathname;
		child->m_parent = this;
		m_children[child->m_pathname] = child;
	}
	
	Unit* Unit::find(std::string path, bool create)
	{
		// get first dir
		std::string subpath;
		size_t pos = path.find("/");
		if (pos == std::string::npos) {
			// no further subpath 
			subpath = path;
			auto unit_it = m_children.find(path);
			if (unit_it == m_children.end())
				// TODO if create
				return NULL;
			return unit_it->second;
		}
		else {
			if (!pos && !m_parent) {
				// handle special case for root
				if (path.substr(0, 2) == "//") {					
					path = path.substr(2);
					pos = path.find("/");
					if (pos == std::string::npos) {
						// no further subpath 
						subpath = path;
						auto unit_it = m_children.find(path);
						if (unit_it == m_children.end())
							// TODO if create
							return NULL;
						return unit_it->second;
					}
				}
			}
			subpath = path.substr(0, pos);
			auto unit_it = m_children.find(subpath);
			if (unit_it == m_children.end())
				// TODO if create
				return NULL;
			return unit_it->second->find(path.substr(pos + 1, path.length()), create);
		}
	}

	void Unit::logMe()
	{
		printf("Name: %s\nPath: %s\nType: %s\n--\n", m_pathname.c_str(), m_path.c_str(), m_type_name.c_str());
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Unit* c = it->second;
			c->logMe();
		}
	}
}