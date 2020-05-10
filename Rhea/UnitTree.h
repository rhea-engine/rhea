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
	public:
		typedef std::function<Unit* (std::string, std::string)> UnitCreatorFunc;

		UnitTree(Application* app) :
			m_app(app),
			Unit("/", "//core/unittree")
		{}

		// create a reource
		Unit* createUnit(std::string basename, std::string type_name, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		// create a resource with res id and no values
		Unit* createUnitById(std::string resource_id, std::string type_name);

		// register a type
		void registerUnitType(std::string type_name, UnitCreatorFunc create_func);

	protected:
		Application* m_app;
		std::unordered_map<std::string, UnitCreatorFunc> m_factory;
	};
}

#endif // !RHEA_UnitTREE_H
