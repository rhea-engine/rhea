#include "UnitTree.h"
#include "SimpleParser/Type.h"


namespace Rhea
{
	Unit* UnitTree::createUnit(std::string basename, std::string type_name)
	{
		Unit* unit = static_cast<Unit*>(SimpleParser::Type::create(basename, type_name));
		if (unit->getType() != SimpleParser::Type::get(SimpleParser::Type::BaseType::NONE))
		{
			unit->setup(m_app);
			return unit;
		}
		else {
#ifndef _DEBUG
			return nullptr;
#else
			throw;
#endif // !_DEBUG
		}
	}

	Unit* UnitTree::createUnit(std::string basename, std::string type_name, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit* unit = static_cast<Unit*>(SimpleParser::Type::create(basename, type_name));
		if (unit->getType() != SimpleParser::Type::get(SimpleParser::Type::BaseType::NONE))
		{
			unit->setup(m_app, attributes, values);
			return unit;
	}
		else {
#ifndef _DEBUG
			return nullptr;
#else
			abort();
#endif // !_DEBUG
		}
	}

	Unit* UnitTree::createUnitById(std::string resource_id, std::string type_name)
	{
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
		unit_parent = doCast<Unit>(find(path.substr(0, rpos)));
		if (!unit_parent)
			return NULL;

		// create unit
		Unit* unit = createUnit(path.substr(rpos + 1), type_name);

		// add it to tree
		if (unit)
			unit_parent->addChild(unit);

		return unit;
	}

	// note: Components cannot be targets
	UnitTree::Binding UnitTree::getBinding(std::string binding_desc, std::string basepath)
	{
		Binding binding = { nullptr, std::string("") };

		size_t pos = binding_desc.find(".");
		if (pos != std::string::npos && pos) {
			if (basepath != "" && (binding_desc.size() < 2 || (binding_desc[0] != '/' && binding_desc[1] != '/')))
				binding.target = doCast<Unit>(find(basepath + "/" + binding_desc.substr(0, pos)));
			else
				binding.target = doCast<Unit>(find(binding_desc.substr(0, pos)));

			binding.value = binding_desc.substr(pos + 1);
		}
		else {
			binding.target = doCast<Unit>(find(basepath));
			if (!pos)
				binding.value = binding_desc.substr(pos + 1); // if someone put a "." at start
			else
				binding.value = binding_desc;
		}
		return binding;
	}
}
