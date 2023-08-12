#ifndef RHEA_UnitTREE_H
#define RHEA_UnitTREE_H

#include <string>
#include <unordered_map>
#include <functional>

#include "Unit.h"

namespace Rhea
{
	class Application;

	// the class that holds a Unit tree 
	// it is also a Unit factory
	class UnitTree : public Unit
	{
		MECLASS(UnitTree, Unit)
	public:
		UnitTree(std::string basename, std::string type_name) :
			Unit(basename, type_name)
		{}

		// create a reource
		Unit* createUnit(std::string basename, std::string type_name, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		
		// create resource without values
		Unit* createUnit(std::string basename, std::string type_name);

		// create a resource with res id and no values
		Unit* createUnitById(std::string resource_id, std::string type_name);

	};
}

#endif // !RHEA_UnitTREE_H
