/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef SIMPLEPARSER_VARIANT_H
#define SIMPLEPARSER_VARIANT_H

#include <string>
#include <vector>
#include "Type.h"

namespace simpleparser
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
		Variant(const std::string& s);
		Variant(std::vector<Variant>& arr);
		Variant(const Variant& variant);
		Variant(const char* s);
		Variant(Variant* obj);

		virtual ~Variant();

		Variant& operator=(const Variant& other);

		// named conversions
		std::string toString() const;
		int toInt() const;
		int64_t toInt64() const;
		float toFloat() const;
		bool toBool() const;
		std::vector<Variant> toArray() const;
		Variant* toObject() const;
		void* toPointer() const;

		// explicit converters
		explicit operator std::string() const { return toString(); }
		operator int() const { return toInt(); }
		operator float() const { return toFloat(); }
		operator bool() const { return toBool(); }
		operator Variant* () const { return toObject(); }
		Variant& operator[](int);
		const Variant& operator[](int i) const;

		/*		operator std::string() const;
				operator std::vector<Variant>() const;
				operator Variant*() const;
				operator float() const;
				operator bool() const;
				operator int() const;
				Variant& operator[](int);
				void operator=(const Variant& variant);
		*/
		// comparison
		bool operator==(const Variant& variant) const;
		bool operator!=(const Variant& variant) const;
		bool isNull() const;

		// support functions
		int length();

		int size() { return length(); }

		// type info
		Type* getType() { return m_type; }
		const Type* getType() const { return m_type; }
		std::string getTypeName() const;
		bool isA(std::string type_name) { return Type::get(type_name) == m_type; }
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
		void clear();
		void copyFrom(const Variant& other);

		union {
			bool _bool;
			int64_t _int;
			float _float;
			void* _ptr;
			std::string* _str;
			std::vector<Variant>* _array;
			Variant* _obj;
		} m_data;

		Type* m_type = nullptr;

		static Variant* s_nullVariant;
	};
}

template<class T>
T* isCast(simpleparser::Variant* src)
{
	return src && (src->getType() == T::staticType()) ? (T*)src : nullptr;

};

template<class T>
T* doCast(simpleparser::Variant* src)
{
	if (!src) return nullptr;
	if (src->getType() == T::staticType()) return (T*)src;
	simpleparser::Type* p = src->getType()->getParent();
	while (p) {
		if (p == T::staticType()) return (T*)src;
		p = p->getParent();
	}
	return nullptr;
}
#endif // !SIMPLEPARSER_VARIANT_H
