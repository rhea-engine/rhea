#include "Variant.h"

namespace SimpleParser
{
	Variant::Variant(bool _bool)
	{
		m_data._bool = _bool;
		m_type = Type::BOOL;
	}

	Variant::Variant(int i)
	{
		m_data._int = i;
		m_type = Type::INT;
	}
	
	Variant::Variant(int64_t i)
	{
		m_data._int = i;
		m_type = Type::INT;
	}
	
	Variant::Variant(float f)
	{
		m_data._float = f;
		m_type = Type::FLOAT;
	}
	
	Variant::Variant(double f)
	{
		m_data._float = f;
		m_type = Type::FLOAT;
	}
	
	Variant::Variant(void* ptr)
	{
		m_data._ptr = ptr;
		m_type = Type::PTR;
	}
	
	Variant::Variant(std::string& s)
	{
		m_data._str = new std::string(s);
		m_type = Type::STRING;
	}
	
	Variant::Variant(std::vector<Variant>& arr)
	{
		m_data._array = new std::vector<Variant>(arr);
		m_type = Type::ARRAY;
	}
	
	Variant::Variant(const Variant& variant)
	{
		m_type = variant.m_type;
		m_data._int = 0;

		switch (m_type) {
		case Type::BOOL:
			m_data._bool = variant.m_data._bool;
			break;
		case Type::INT:
			m_data._bool = variant.m_data._bool;
			break;
		case Type::FLOAT:
			m_data._float = variant.m_data._float;
			break;
		case Type::PTR:
			m_data._ptr = variant.m_data._ptr;
			break;
		case Type::STRING:
			m_data._str = new std::string(*variant.m_data._str);
			break;
		case Type::ARRAY:
			m_data._array = new std::vector<Variant>;
			for (auto it = variant.m_data._array->begin(); it != variant.m_data._array->end(); ++it) {
				m_data._array->push_back(*it);
			}
			break;
		}
	}

	Variant::Variant(const char* s)
	{
		m_data._str = new std::string(s);
		m_type = Type::STRING;
	}

	Variant::~Variant()
	{
		switch (m_type) {
		case Type::STRING:
			delete m_data._str;
			break;
		case Type::ARRAY:
			delete m_data._array;
			break;
		}
	}

	Variant::operator std::string() const {
		switch (m_type) {
		case Type::NONE:
			return "No type";
		case Type::STRING:
			return *m_data._str;
		default:
			return "type to string not implemented";
		}
	}

	Variant::operator std::vector<Variant>() const
	{
		if (m_type == Type::ARRAY)
			return *m_data._array;
		else
			return std::vector<Variant>();
	}
	
	Variant::operator float() const
	{
		switch (m_type) {
		case Type::BOOL:
			return (m_data._bool) ? 1 : 0;
		case Type::FLOAT:
			return m_data._float;
		case Type::INT:
			return (float)m_data._int;
		default:
			return 0.0;
		}
	}

	Variant::operator bool() const
	{
		switch (m_type) {
		case Type::BOOL:
			return m_data._bool;
		case Type::FLOAT:
			return (m_data._float) ? true : false;
		case Type::INT:
			return (m_data._int) ? true : false;
		case Type::STRING:
			return (m_data._str->compare("true") ) ? false : true;
		default:
			return false;
		}
	}

	Variant& Variant::operator[](int i)
	{
		if (m_type != Type::ARRAY || i >= m_data._array->size())
			return Variant();

		return (*m_data._array)[i];
	}

	void Variant::operator=(const Variant& variant)
	{
		m_type = variant.m_type;
		m_data._int = 0;

		switch (m_type) {
		case Type::BOOL:
			m_data._bool = variant.m_data._bool;
			break;
		case Type::INT:
			m_data._bool = variant.m_data._bool;
			break;
		case Type::FLOAT:
			m_data._float = variant.m_data._float;
			break;
		case Type::PTR:
			m_data._ptr = variant.m_data._ptr;
			break;
		case Type::STRING:
			m_data._str = new std::string(*variant.m_data._str);
			break;
		case Type::ARRAY:
			m_data._array = new std::vector<Variant>;
			for (auto it = variant.m_data._array->begin(); it != variant.m_data._array->end(); ++it) {
				m_data._array->push_back(*it);
			}
			break;		
		}
	}

	bool Variant::operator==(const Variant& variant) const
	{
		if (m_type != variant.m_type)
			return false;
		switch (m_type) {
		case Type::BOOL:
			return m_data._bool == variant.m_data._bool;
		case Type::INT:
			return m_data._int == variant.m_data._int;
		case Type::FLOAT:
			return m_data._float == variant.m_data._float;
		case Type::PTR:
			return m_data._ptr == variant.m_data._ptr;
		case Type::STRING:
			return *m_data._str == *variant.m_data._str;
		default:
			return false;
		}
	}

	bool Variant::operator!=(const Variant& variant) const
	{
		if (m_type != variant.m_type)
			return true;
		switch (m_type) {
		case Type::BOOL:
			return m_data._bool != variant.m_data._bool;
		case Type::INT:
			return m_data._int != variant.m_data._int;
		case Type::FLOAT:
			return m_data._float != variant.m_data._float;
		case Type::PTR:
			return m_data._ptr != variant.m_data._ptr;
		case Type::STRING:
			return *m_data._str != *variant.m_data._str;
		default:
			return false;
		}
	}
	
	int Variant::length()
	{
		switch (m_type) {
		case Type::STRING:
			return m_data._str->length();
			break;
		case Type::ARRAY:
			return m_data._array->size();
			break;
		default:
			return 0;
		}
	}
}