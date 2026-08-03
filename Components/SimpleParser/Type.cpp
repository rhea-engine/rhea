/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Type.h"
#include "Variant.h"

namespace simpleparser
{
	Type* Type::Register(std::string name, CreatorFunc factory)
	{
		if (m_types.find(name) != m_types.end())
			return m_types[name];

		Type* type = new Type(BaseType::OBJECT, name, factory);
		m_types[name] = type;
		return type;
	}

	void Type::registerProperty(std::string name, Type* unit_type, std::string setter, std::string getter)
	{
		m_properties[name] = new Property{ unit_type, setter, getter };
	}

	void Type::registerMethod(std::string name, Method* method)
	{
		m_methods[name] = method;
	}
	
	Type* Type::getPropertyType(std::string& name)
	{
		auto it = m_properties.find(name);
		if (it == m_properties.end()) {
			// not found
			return nullptr;
		}
		else {
			return it->second->type;
		}
	}

	Variant* Type::create(std::string basename, std::string type_name)
	{
		//  find factory 
		auto f = m_types.find(type_name);
		Variant* var;
		if (f == m_types.end()) {
			// type not found - create a base type
			var = new Variant();
		}
		else {
			CreatorFunc func = f->second->m_factory;
			var = func(basename, type_name);
		}
		return var;
	}

	Type* Type::get(BaseType t)
	{
		if (m_base_types.find(t) != m_base_types.end())
			return m_base_types[t];
		else
			return m_base_types[Type::BaseType::NONE];
	}

	Type* Type::get(const std::string& type_name)
	{
		auto it = m_types.find(type_name);
		if (it != m_types.end())
			return it->second;
#ifndef _DEBUG
		return m_base_types[Type::BaseType::NONE];
#else
		throw("Unknown Type");
#endif
	}

	//  type database
	std::unordered_map<std::string, Type*> Type::m_types;

	std::map<Type::BaseType, Type*> Type::m_base_types = {
		{ Type::BaseType::NONE, new Type{Type::BaseType::NONE, std::string("//core/types/none"), Variant::createType} },
		{ Type::BaseType::BOOL, new Type{Type::BaseType::BOOL, std::string("//core/types/bool"), Variant::createType} },
		{ Type::BaseType::INT, new Type{Type::BaseType::INT, std::string("//core/types/int"), Variant::createType} },
		{ Type::BaseType::FLOAT, new Type{Type::BaseType::FLOAT, std::string("//core/types/float"), Variant::createType} },
		{ Type::BaseType::PTR, new Type{Type::BaseType::PTR, std::string("//core/types/ptr"), Variant::createType} },
		{ Type::BaseType::STRING, new Type{Type::BaseType::STRING, std::string("//core/types/string"), Variant::createType} },
		{ Type::BaseType::ARRAY, new Type{Type::BaseType::ARRAY, std::string("//core/types/array"), Variant::createType} },
		{ Type::BaseType::OBJECT, new Type{Type::BaseType::OBJECT, std::string("//core/types/object"), Variant::createType} },
	};
}
