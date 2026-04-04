#include "Rhea/Component.h"
#include "Rhea/Unit.h"
#include <SimpleParser/Method.h>

namespace rhea
{
	Component* Component::createType(std::string name, std::string type_name)
	{
		Component* ret = new Component(name, type_name);
		return ret;
	}

	void Component::initializeType(SimpleParser::Type* type)
	{
		type->registerMethod("setName", &Component::setName);
		type->registerMethod("getName", &Component::getPathName);

		type->registerProperty("path", SimpleParser::Type::get(SimpleParser::Type::BaseType::STRING), "setName", "getName");
	}

	void Component::setName(std::string name)
	{
		// update the full path
		if (m_parent)
			m_parent->removeChild(this);

		// update my unit name
		m_pathname = name;

		if (m_parent)
			m_parent->addChild(this);
	}

	Component* Component::getCommonAncestor(Component* u2)
	{
		if (!u2)
			return nullptr;

		Component* u1 = this;

		if (u1 == u2)
			return u1;

		// if u1 is already anchestor u2 is the result
		if (u1 && u1->isAncestor(u2))
			return u2;

		while (u1)
		{
			if (u2->isAncestor(u1))
				break;

			u1 = u1->m_parent;
		}

		return u1;
	}

	bool Component::isAncestor(Component* u2)
	{
		// if we have no parent we have no ancestor
		if (!m_parent)
			return false;

		// check our immediate parent
		if (m_parent == u2)
			return true;

		// not our parent - check back up the tree
		return m_parent->isAncestor(u2);
	}


}