#ifndef SIMPLEPARSER_TYPE_H
#define SIMPLEPARSER_TYPE_H

#include <string>
#include <unordered_map>
#include <map>
#include <functional>

#include "Variant.h"

namespace SimpleParser
{
	class Method;

	class Type
	{
	public:
		enum class BaseType
		{
			NONE,
			BOOL,
			INT,
			FLOAT,
			PTR,
			STRING,
			ARRAY,
			OBJECT
		};

		struct Property
		{
			Type* type;
			std::string setter;
			std::string getter;
		};

		typedef std::function<Variant* (std::string, std::string)> CreatorFunc;
		Type(std::string name, CreatorFunc factory) :
			Type(BaseType::OBJECT, name, factory)
		{}
		Type(BaseType base, std::string name, CreatorFunc factory) :
			m_base(base),
			m_name(name),
			m_factory(factory)
		{
			m_types[name] = this;
		}
		static Type* Register(std::string name, CreatorFunc factory);

		// call initialize and register type by class
		template <class T>
		static void registerType(std::string URI)
		{
			Type* type = Type::Register(URI, T::createType);
			T::initializeType(type);
		}

		// get name of type
		std::string& getName() { return m_name; }

		// register a new property with the given type
		void registerProperty(std::string name, Type* unit_type, std::string setter, std::string getter);

		// lookup a property type with a given name
		Type* getPropertyType(std::string& name);

		// get all properties
		std::unordered_map<std::string, Property*> getProperties() { return m_properties; }

		// register a new method with the given type
		void registerMethod(std::string name, Method* method);
		template<class M>
		void registerMethod(std::string name, M method)
		{
			auto mth = Method::bindMethod(method);
			registerMethod(name, mth);
		}

		// get a method 
		Method* getMethod(std::string name) { return m_methods[name]; }

		// type database

		// creation
		static Variant* create(std::string basename, std::string type_name);

		static Type* get(BaseType t);
		static Type* get(std::string& type_name);
		
		friend class Variant;

	protected:
		BaseType m_base;
		std::string m_name;
		std::unordered_map<std::string, Property*> m_properties;
		CreatorFunc m_factory;
		std::unordered_map<std::string, Method*> m_methods;

		// type database
		static std::unordered_map<std::string, Type*> m_types;
		static std::map<BaseType, Type*> m_base_types;
	};
}

#endif // !SIMPLEPARSER_TYPE_H
