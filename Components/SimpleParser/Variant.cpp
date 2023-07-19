#include "Variant.h"
#include "Type.h"

namespace SimpleParser
{
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
	
	Variant::Variant(std::string& s)
	{
		m_data._str = new std::string(s);
		m_type = Type::m_base_types[Type::BaseType::STRING];
	}
	
	Variant::Variant(std::vector<Variant>& arr)
	{
		m_data._array = new std::vector<Variant>(arr);
		m_type = Type::m_base_types[Type::BaseType::ARRAY];
	}
	
	Variant::Variant(const Variant& variant)
	{
		m_type = variant.m_type;
		m_data._int = 0;

		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			m_data._bool = variant.m_data._bool;
			break;
		case Type::BaseType::INT:
			m_data._int = variant.m_data._int;
			break;
		case Type::BaseType::FLOAT:
			m_data._float = variant.m_data._float;
			break;
		case Type::BaseType::PTR:
			m_data._ptr = variant.m_data._ptr;
			break;
		case Type::BaseType::STRING:
			m_data._str = new std::string(*variant.m_data._str);
			break;
		case Type::BaseType::ARRAY:
			m_data._array = new std::vector<Variant>;
			for (auto it = variant.m_data._array->begin(); it != variant.m_data._array->end(); ++it) {
				m_data._array->push_back(*it);
			}
			break;
		case Type::BaseType::OBJECT:
			m_data._obj = variant.m_data._obj;
			break;
		}
	}

	Variant::Variant(const char* s)
	{
		m_data._str = new std::string(s);
		m_type = Type::m_base_types[Type::BaseType::STRING];
	}

	Variant::Variant(Variant* obj)
	{
		m_data._obj = obj;
		m_type = Type::m_base_types[Type::BaseType::OBJECT];
	}

	Variant::~Variant()
	{
		if (m_type) {
			switch (m_type->m_base) {
			case Type::BaseType::STRING:
				delete m_data._str;
				break;
			case Type::BaseType::ARRAY:
				delete m_data._array;
				break;
			}
		}
	}

	Variant::operator std::string() const {
		switch (m_type->m_base) {
		case Type::BaseType::NONE:
			return "No type";
		case Type::BaseType::STRING:
			return *m_data._str;
		default:
			return "type to string not implemented";
		}
	}

	
	Variant::operator std::vector<Variant>() const
	{
		if (m_type->m_base == Type::BaseType::ARRAY)
			return *m_data._array;
		else
			return std::vector<Variant>();
	}
	
	Variant::operator Variant*() const
	{
		if (m_type->m_base == Type::BaseType::OBJECT)
			return m_data._obj;
		else
			return nullptr;
	}

	Variant::operator float() const
	{
		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			return (m_data._bool) ? 1 : 0;
		case Type::BaseType::FLOAT:
			return m_data._float;
		case Type::BaseType::INT:
			return (float)m_data._int;
		default:
			return 0.0;
		}
	}

	Variant::operator bool() const
	{
		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			return m_data._bool;
		case Type::BaseType::FLOAT:
			return (m_data._float) ? true : false;
		case Type::BaseType::INT:
			return (m_data._int) ? true : false;
		case Type::BaseType::STRING:
			return (m_data._str->compare("true") ) ? false : true;
		default:
			return false;
		}
	}

	Variant::operator int() const
	{
		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			return (m_data._bool) ? 1 : 0;
		case Type::BaseType::FLOAT:
			return (int)m_data._float;
		case Type::BaseType::INT:
			return m_data._int;
		default:
			return 0;
		}
	}

	Variant& Variant::operator[](int i)
	{
		if (m_type->m_base != Type::BaseType::ARRAY || i >= m_data._array->size())
			return Variant();

		return (*m_data._array)[i];
	}

	void Variant::operator=(const Variant& variant)
	{
		m_type = variant.m_type;
		m_data._int = 0;

		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			m_data._bool = variant.m_data._bool;
			break;
		case Type::BaseType::INT:
			m_data._int = variant.m_data._int;
			break;
		case Type::BaseType::FLOAT:
			m_data._float = variant.m_data._float;
			break;
		case Type::BaseType::PTR:
			m_data._ptr = variant.m_data._ptr;
			break;
		case Type::BaseType::STRING:
			m_data._str = new std::string(*variant.m_data._str);
			break;
		case Type::BaseType::ARRAY:
			m_data._array = new std::vector<Variant>;
			for (auto it = variant.m_data._array->begin(); it != variant.m_data._array->end(); ++it) {
				m_data._array->push_back(*it);
			}
			break;	
		case Type::BaseType::OBJECT:
			m_data._obj = variant.m_data._obj;
			break;
		}
	}

	bool Variant::operator==(const Variant& variant) const
	{
		if (m_type != variant.m_type)
			return false;
		switch (m_type->m_base) {
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

	bool Variant::operator!=(const Variant& variant) const
	{
		if (m_type != variant.m_type)
			return true;
		switch (m_type->m_base) {
		case Type::BaseType::BOOL:
			return m_data._bool != variant.m_data._bool;
		case Type::BaseType::INT:
			return m_data._int != variant.m_data._int;
		case Type::BaseType::FLOAT:
			return m_data._float != variant.m_data._float;
		case Type::BaseType::PTR:
			return m_data._ptr != variant.m_data._ptr;
		case Type::BaseType::STRING:
			return *m_data._str != *variant.m_data._str;
		case Type::BaseType::OBJECT:
			return m_data._obj != variant.m_data._obj;
		default:
			return false;
		}
	}

	bool Variant::isNull() const
	{
		return m_type->m_base == Type::BaseType::NONE;
	}
	
	int Variant::length()
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

	std::string& Variant::getTypeName()
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