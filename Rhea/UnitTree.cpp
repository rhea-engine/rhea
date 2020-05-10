#include "UnitTree.h"

namespace Rhea
{
	Unit* UnitTree::createUnit(std::string basename, std::string type_name, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		//  find factory 
		std::unordered_map<std::string, UnitCreatorFunc>::const_iterator f = m_factory.find(type_name);
		Unit* unit;
		if (f == m_factory.end()) {
			// type not found - create a base unit type
			unit = new Unit(basename, type_name);
		}
		else {
			UnitCreatorFunc func = f->second;
			unit = func(basename, type_name);
		}
		unit->setup(m_app, attributes, values);
		return unit;
	}

	Unit* UnitTree::createUnitById(std::string resource_id, std::string type_name)
	{
		//  find factory 
		std::unordered_map<std::string, UnitCreatorFunc>::const_iterator f = m_factory.find(type_name);
		Unit* unit;

		// basename / path
		// split path into basename and path
		std::string path = resource_id;
		size_t rpos = path.find_last_of("/");
		if (rpos == path.length()) {
			// trailing delim
			rpos = path.find_last_of("/", 1);
		}
		if (rpos == path.size() || rpos == std::string::npos)
			// invalid path
			return NULL;

		// find parent
		Unit* unit_parent;
		unit_parent = find(path.substr(0, rpos));
		if (!unit_parent)
			return NULL;

		// create unit
		if (f == m_factory.end()) {
			// type not found - create a base unit type
			unit = new Unit(path.substr(rpos+1), type_name);
		}
		else {
			UnitCreatorFunc func = f->second;
			unit = func(path.substr(rpos + 1), type_name);
		}
		unit->setup(m_app);

		// add it to tree
		unit_parent->addChild(unit);

		return unit;
	}

	void UnitTree::registerUnitType(std::string type_name, UnitCreatorFunc create_func)
	{
		m_factory[type_name] = create_func;
	}
}
