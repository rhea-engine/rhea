/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Component.h>
#include <Rhea/Unit.h>
#include <SimpleParser/Method.h>
#include <Rhea/Resources/ResourceContext.h>

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

		for (auto* type = getType(); type; type = type->getParent()) {
			const auto properties_it = s_resource_properties.find(type);
			if (properties_it == s_resource_properties.end())
				continue;

			for (const auto& property : properties_it->second) {
				const auto value_it = m_values.find(property.name);
				if (value_it == m_values.end())
					continue;

				property.set_id(*this, value_it->second.toString());
			}
		}
	}

	void Component::destroy()
	{
		if (m_destroying)
			return;

		m_destroying = true;

		if (m_parent)
			m_parent->removeChild(this);

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

	void Component::resolveReferences(const std::string& context_root, const std::string& source_root)
	{
		const ResourceContext context{ context_root, source_root };
		const std::string base_path = getAttribute("basepath");

		for (auto* type = getType(); type; type = type->getParent()) {
			const auto properties_it = s_resource_properties.find(type);
			if (properties_it == s_resource_properties.end())
				continue;

			for (const auto& property : properties_it->second) {
				if (!base_path.empty())
					property.set_base_path(*this, base_path);

				const std::string id = property.get_id(*this);

				// Preserve empty IDs, local object references and explicit realms.
				if (id.empty() || id[0] != '/' || id.compare(0, 2, "//") == 0)
					continue;

				std::string rid;
				if (context.normalizeResourceId(id, rid))
					property.set_id(*this, rid);
			}
		}
	}

	std::vector<Component::ResourceReference> Component::getReferences()
	{
		std::vector<ResourceReference> references;

		for (auto* type = getType(); type; type = type->getParent()) {
			const auto properties_it = s_resource_properties.find(type);
			if (properties_it == s_resource_properties.end())
				continue;

			for (const auto& property : properties_it->second) {
				const std::string id = property.get_id(*this);
				if (id.empty())
					continue;

				references.push_back({
					property.name,
					property.resource_type,
					id,
					property.usage
					});
			}
		}

		return references;
	}

	Component* Component::instance(const std::string& name) 
	{
		InstantiationContext context;
		Component* result = instanceCreate(name, this, context);

		if (!result)
			return nullptr;

		result->setOrigin(ComponentOrigin::Configured);
		instanceSetup(this, result, context);

		return result;
	}

	Component* Component::instanceCreate(const std::string& name, Component* source, InstantiationContext& context)
	{
		std::string instance_name = name.empty() ? source->getPathName() : name;

		Component* result = doCast<Component>(simpleparser::Type::create(instance_name, source->getTypeName()));

		if (!result)
			return nullptr;

		result->setOrigin(ComponentOrigin::Configured);
		context.instantiated_components[source] = result;
		return result;
	}

	void Component::instanceSetup(Component* source, Component* target, InstantiationContext& context)
	{
		auto values = source->getValues();
		values.erase("fabric");
		target->setup(m_app, source->getAttributes(), values);
		copyResourceProperties(*source, *target);
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
	
	void Component::copyResourceProperties(Component& source, Component& target)
	{
		for (auto* type = source.getType(); type; type = type->getParent()) {
			const auto properties_it = s_resource_properties.find(type);
			if (properties_it == s_resource_properties.end())
				continue;

			for (const auto& property : properties_it->second)
				property.copy(source, target);
		}
	}

	std::unordered_map<simpleparser::Type*, std::vector<Component::ResourceProperty>> Component::s_resource_properties;

	std::string Component::normalizeReferenceId(
		const std::string& id,
		const ResourceContext& context) const
	{
		if (id.empty() || id[0] != '/' || id.compare(0, 2, "//") == 0)
			return id;

		std::string rid;
		if (context.normalizeResourceId(id, rid))
			return rid;
		return id;
	}
}