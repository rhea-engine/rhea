#ifndef SIMPLEPARSER_VARIANT_H
#define SIMPLEPARSER_VARIANT_H

#include <string>
#include <vector>

namespace SimpleParser
{
	class Variant
	{
	public:

		enum class Type
		{
			NONE,
			BOOL,
			INT,
			FLOAT,
			PTR,
			STRING,
			ARRAY
		};

		Variant() : m_type(Type::NONE) { m_data._int = 0; }
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

		~Variant();

		// converters
		operator std::string() const;
		operator std::vector<Variant>() const;
		operator float() const;
		operator bool() const;
		operator int() const;
		Variant& operator[](int);
		void operator=(const Variant& variant);

		// comparison
		bool operator==(const Variant& variant) const;
		bool operator!=(const Variant& variant) const;

		// support functions
		int length();
		int size() { return length(); }

	protected:
		union {
			bool _bool;
			int64_t _int;
			float _float;
			void* _ptr;
			std::string* _str;
			std::vector<Variant>* _array;
		} m_data;

		Type m_type;
	};
}

#endif // !SIMPLEPARSER_VARIANT_H
