#ifndef SIMPLEPARSER_VARIANT_H
#define SIMPLEPARSER_VARIANT_H

#include <string>
#include <vector>
#include "Type.h"

namespace SimpleParser
{
	class Variant
	{
	public:

		Variant();
		Variant(Type* t) : m_type(t) { m_data._int = 0; }
		Variant(bool _bool);
		Variant(int i);
		Variant(int64_t i);
		Variant(float f);
		Variant(double f);
		Variant(void* ptr);
		Variant(std::string& s);
		Variant(std::vector<Variant>& arr);
		Variant(const Variant& variant);
		Variant(const char* s);
		Variant(Variant* obj);

		virtual ~Variant();

		// converters
		operator std::string() const;
		operator std::vector<Variant>() const;
		operator Variant*() const;
		operator float() const;
		operator bool() const;
		operator int() const;
		Variant& operator[](int);
		void operator=(const Variant& variant);

		// comparison
		bool operator==(const Variant& variant) const;
		bool operator!=(const Variant& variant) const;
		bool isNull() const;

		// support functions
		int length();
		
		int size() { return length(); }

		// type info
		Type* getType() { return m_type; }
		std::string getTypeName() const; 
		bool isA(std::string type) { return !getTypeName().compare(type); }
		bool isArray() { return (m_type->m_base == Type::BaseType::ARRAY); }

		// factory
		static Variant* createType(std::string name, std::string type);
		// type helper
		static Type* staticType(Type* type = nullptr) {
			static Type* _static_type = nullptr;
			if (type)
				_static_type = type;
			return _static_type;
		}

	protected:
		union {
			bool _bool;
			int64_t _int;
			float _float;
			void* _ptr;
			std::string* _str;
			std::vector<Variant>* _array;
			Variant* _obj;
		} m_data;

		Type* m_type;
	};
}

#endif // !SIMPLEPARSER_VARIANT_H
