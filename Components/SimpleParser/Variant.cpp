/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Variant.h"
#include "Variant.h"
#include "Type.h"

namespace simpleparser
{
	Variant* Variant::s_nullVariant = nullptr;

	Variant::Variant() :
		Variant(Type::m_base_types[Type::BaseType::NONE])
	{
	}

	Variant::Variant(bool _bool)
	{
		m_data._bool = _bool;
		m_type = Type::m_base_types[Type::BaseType::BOOL];
	}

	Variant::Variant(int i)
	{
		m_data._int = i;
		m_type = Type::m_base_types[Type::BaseType::INT];
	}
	
	Variant::Variant(int64_t i)
	{
		m_data._int = i;
		m_type = Type::m_base_types[Type::BaseType::INT];
	}
	
	Variant::Variant(float f)
	{
		m_data._float = f;
		m_type = Type::m_base_types[Type::BaseType::FLOAT];
	}
	
	Variant::Variant(double f)
	{
		m_data._float = f;
		m_type = Type::m_base_types[Type::BaseType::FLOAT];
	}
	
	Variant::Variant(void* ptr)
	{
		m_data._ptr = ptr;
		m_type = Type::m_base_types[Type::BaseType::PTR];
	}
	
	Variant::Variant(const std::string& s)
	{
		m_data._str = new std::string(s);
		m_type = Type::get(Type::BaseType::STRING);
	}
	
	Variant::Variant(std::vector<Variant>& arr)
	{
		m_data._array = new std::vector<Variant>(arr);
		m_type = Type::m_base_types[Type::BaseType::ARRAY];
	}
	
	void Variant::copyFrom(const Variant& other)
	{
		m_type = other.m_type;
		m_data._int = 0;

		if (!m_type)
			return;

		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			m_data._bool = other.m_data._bool;
			break;
		case Type::BaseType::INT:
			m_data._int = other.m_data._int;
			break;
		case Type::BaseType::FLOAT:
			m_data._float = other.m_data._float;
			break;
		case Type::BaseType::PTR:
			m_data._ptr = other.m_data._ptr;
			break;
		case Type::BaseType::STRING:
			m_data._str = new std::string(*other.m_data._str);
			break;
		case Type::BaseType::ARRAY:
			m_data._array = new std::vector<Variant>(*other.m_data._array);
			break;
		case Type::BaseType::OBJECT:
			m_data._obj = other.m_data._obj;
			break;
		case Type::BaseType::NONE:
			break;
		}
	}

	Variant::Variant(const char* s)
	{
		m_data._str = new std::string(s ? s : "");
		m_type = Type::m_base_types[Type::BaseType::STRING];
	}

	Variant::Variant(Variant* obj)
	{
		m_data._obj = obj;
		m_type = Type::m_base_types[Type::BaseType::OBJECT];
	}

	Variant::Variant(const Variant& other)
	{
		copyFrom(other);
	}

	Variant::~Variant()
	{
		clear();
	}

	Variant& Variant::operator=(const Variant& other)
	{
		if (this != &other) {
			clear();
			copyFrom(other);
		}
		return *this;
	}

	void Variant::clear()
	{
		if (!m_type)
			return;

		switch (m_type->m_base) {
		case Type::BaseType::STRING:
			delete m_data._str;
			break;
		case Type::BaseType::ARRAY:
			delete m_data._array;
			break;
		default:
			break;
		}

		m_data._int = 0;
		m_type = Type::m_base_types[Type::BaseType::NONE];
	}

	std::string Variant::toString() const
	{
		if (!m_type)
			return "";

		switch (m_type->m_base) {
		case Type::BaseType::NONE:
			return "No Type";
		case Type::BaseType::BOOL:
			return m_data._bool ? "true" : "false";
		case Type::BaseType::INT:
			return std::to_string(m_data._int);
		case Type::BaseType::FLOAT:
			return std::to_string(m_data._float);
		case Type::BaseType::STRING:
			return *m_data._str;
		default:
			return "";
		}
	}
	
	float Variant::toFloat() const
	{
		if (!m_type)
			return 0.0f;

		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			return m_data._bool ? 1.0f : 0.0f;
		case Type::BaseType::INT:
			return static_cast<float>(m_data._int);
		case Type::BaseType::FLOAT:
			return m_data._float;
		default:
			return 0.0f;
		}
	}

	bool Variant::toBool() const
	{
		if (!m_type)
			return false;

		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			return m_data._bool;
		case Type::BaseType::INT:
			return m_data._int != 0;
		case Type::BaseType::FLOAT:
			return m_data._float != 0.0f;
		case Type::BaseType::STRING:
			return m_data._str && (*m_data._str == "true" || *m_data._str == "1");
		default:
			return false;
		}
	}

	int Variant::toInt() const
	{
		if (!m_type)
			return 0;

		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			return m_data._bool ? 1 : 0;
		case Type::BaseType::INT:
			return static_cast<int>(m_data._int);
		case Type::BaseType::FLOAT:
			return static_cast<int>(m_data._float);
		default:
			return 0;
		}
	}

	int64_t Variant::toInt64() const
	{
		if (!m_type)
			return 0;

		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			return m_data._bool ? 1 : 0;
		case Type::BaseType::INT:
			return m_data._int;
		case Type::BaseType::FLOAT:
			return static_cast<int64_t>(m_data._float);
		default:
			return 0;
		}
	}

	std::vector<Variant> Variant::toArray() const
	{
		if (m_type && m_type->m_base == Type::BaseType::ARRAY)
			return *m_data._array;
		return {};
	}

	Variant* Variant::toObject() const
	{
		if (m_type && m_type->m_base == Type::BaseType::OBJECT)
			return m_data._obj;
		return nullptr;
	}

	void* Variant::toPointer() const
	{
		if (m_type && m_type->m_base == Type::BaseType::PTR)
			return m_data._ptr;
		return nullptr;
	}

	Variant& Variant::operator[](int i)
	{
		if (m_type->m_base != Type::BaseType::ARRAY || i < 0 || i >= static_cast<int>(m_data._array->size())) {
			if (!s_nullVariant)
				s_nullVariant = new Variant();
			return *s_nullVariant;
		}

		return (*m_data._array)[i];
	}

	const Variant& Variant::operator[](int i) const
	{
		if (m_type->m_base != Type::BaseType::ARRAY || i < 0 || i >= static_cast<int>(m_data._array->size())) {
			if (!s_nullVariant)
				s_nullVariant = new Variant();
			return *s_nullVariant;
		}

		return (*m_data._array)[i];
	}

	bool Variant::operator==(const Variant& variant) const
	{
		if (m_type != variant.m_type)
			return false;
		switch (m_type->m_base) {
		case Type::BaseType::NONE:
			return true;
		case Type::BaseType::BOOL:
			return m_data._bool == variant.m_data._bool;
		case Type::BaseType::INT:
			return m_data._int == variant.m_data._int;
		case Type::BaseType::FLOAT:
			return m_data._float == variant.m_data._float;
		case Type::BaseType::PTR:
			return m_data._ptr == variant.m_data._ptr;
		case Type::BaseType::STRING:
			return *m_data._str == *variant.m_data._str;
		case Type::BaseType::OBJECT:
			return m_data._obj == variant.m_data._obj;
		default:
			return false;
		}
	}

	bool Variant::operator!=(const Variant& other) const
	{
		return !(*this == other);
	}

	bool Variant::isNull() const
	{
		return m_type->m_base == Type::BaseType::NONE;
	}
	
	int Variant::length() const
	{
		switch (m_type->m_base) {
		case Type::BaseType::STRING:
			return m_data._str->length();
			break;
		case Type::BaseType::ARRAY:
			return m_data._array->size();
			break;
		default:
			return 0;
		}
	}

	std::string Variant::getTypeName() const
	{
		return m_type->m_name;
	}

	// this function exists for completeness
	// usually you would simply create the base Variant type directly
	Variant* Variant::createType(std::string name, std::string type)
	{
		switch (Type::m_types[name]->m_base) {
		case Type::BaseType::STRING:
			return new Variant(name);
		case Type::BaseType::ARRAY:
			// Note: this would create a memory leak
			// return new Variant(new std::vector<Variant>);
			return nullptr;
		default:
			return new Variant(Type::m_types[name]);
		}
	}
}