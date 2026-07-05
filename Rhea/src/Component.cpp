/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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

	void Component::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values)
	{
		m_app = app;
		m_attributes = attributes;
		m_values = values;
	}

	void Component::destroy()
	{
		if (m_destroying)
			return;

		m_destroying = true;
		onDestroy();
		delete this;
	}

	void Component::initializeType(simpleparser::Type* type)
	{
		type->registerMethod("setName", &Component::setName);
		type->registerMethod("getName", &Component::getPathName);

		type->registerProperty("path", simpleparser::Type::get(simpleparser::Type::BaseType::STRING), "setName", "getName");
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

	Variant Component::getValue(std::string name) const
	{
		if (name != "") {
			auto val = m_values.find(name);
			if (val != m_values.end()) {
				return val->second;
			}
		}
		return Variant();
	}

	std::string Component::getValueString(std::string name, std::string default_val) const
	{
		auto val = m_values.find(name);
		if (val != m_values.end()) {
			return val->second.toString();
		}
		return default_val;
	}

	std::string Component::getValueString(std::string name) const
	{
		if (name != "") {
			auto val = m_values.find(name);
			if (val != m_values.end()) {
				return val->second.toString();
			}
		}
		return std::string("");
	}

	Variant Component::getValue(std::string name, Variant default_val) const
	{
		auto val = m_values.find(name);
		if (val != m_values.end()) {
			return val->second;
		}
		return default_val;
	}

	void Component::setValue(std::string name, Variant val)
	{
		m_values[name] = val;
	}

	std::vector<Component::Export> Component::getReferences()
	{
		return std::vector<Export>();
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