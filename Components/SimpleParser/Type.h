#ifndef SIMPLEPARSER_TYPE_H
#define SIMPLEPARSER_TYPE_H

#include <string>
#include <unordered_map>
#include <map>
#include <functional>

namespace SimpleParser
{
	class Method;
	class Variant;

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
			m_parent(nullptr),
			m_factory(factory)
		{
			m_types[name] = this;
		}
		static Type* Register(std::string name, CreatorFunc factory);

		// register an additional type name for a Type
		// Note: this is proably only useful in creation
		//		 a search would return the real type
		template <class T>
		static void registerType(std::string URI)
		{
			if (m_types.find(URI) != m_types.end())
				return;
			m_types[URI] = T::staticType();
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

		template<typename T>
		static T* create(std::string basename) {
			Type* t = T::staticType();
			if (t)
				return (T*)(t->m_factory(basename, t->getName()));
			else
				return nullptr;
		}

		static Type* get(BaseType t);
		static Type* get(std::string& type_name);

		Type* getParent() { return m_parent; }
		void setParent(Type* parentType) { m_parent = parentType; }

		friend class Variant;

	protected:
		BaseType m_base;
		std::string m_name;
		std::unordered_map<std::string, Property*> m_properties;
		CreatorFunc m_factory;
		std::unordered_map<std::string, Method*> m_methods;
		Type* m_parent;

		// type database
		static std::unordered_map<std::string, Type*> m_types;
		static std::map<BaseType, Type*> m_base_types;		
	};
}

template<class T>
T* isCast(SimpleParser::Variant* src)
{
	return src && (src->getType() == T::staticType()) ? (T*)src : nullptr;

};

template<class T>
T* doCast(SimpleParser::Variant* src)
{
	if (!src) return nullptr;
	if (src->getType() == T::staticType()) return (T*)src;
	SimpleParser::Type* p = src->getType()->getParent();
	while (p) {
		if (p == T::staticType()) return (T*)src;
		p = p->getParent();
	}
	return nullptr;
}

#define RECLASS(_MeClass, _Inherit, _URI)  \
	public: \
static SimpleParser::Type* staticType(SimpleParser::Type* newType = nullptr) { \
	static SimpleParser::Type* _static_type = nullptr; \
	if (!_static_type) { \
		if (!newType) {                                              \
            SimpleParser::Type* parent = _Inherit::staticType();     \
            newType = SimpleParser::Type::Register(_URI, _MeClass::createType); \
            newType->setParent(parent);                              \
            _static_type = newType;                                  \
            _MeClass::initializeType(_static_type);                  \
        }  \
	} \
	return _static_type; \
} \
static void _register_ ## _MeClass() { _MeClass::staticType(); } \
 \
static Component* createType(std::string name, std::string type) \
{ \
	Component* ret = (Component*)new _MeClass(name, type); \
	return ret; \
} \
 \
_MeClass::_MeClass(std::string name) : \
  _MeClass::_MeClass(name, _URI) {} \
 \
private: \


#define RECLASS_VIRTUAL(_MeClass, _Inherit, _URI)  \
	public: \
static SimpleParser::Type* staticType(SimpleParser::Type* newType = nullptr) { \
	static SimpleParser::Type* _static_type = nullptr; \
	if (!_static_type) { \
		if (!newType) {                                              \
            SimpleParser::Type* parent = _Inherit::staticType();     \
            newType = SimpleParser::Type::Register(_URI, _MeClass::createType); \
            newType->setParent(parent);                              \
            _static_type = newType;                                  \
            _MeClass::initializeType(_static_type);                  \
        }  \
	} \
	return _static_type; \
} \
static void _register_ ## _MeClass() { _MeClass::staticType(); } \
 \
private: \


#define MECLASS(_MeClass, _Inherit)  \
	public: \
static SimpleParser::Type* staticType(SimpleParser::Type* newType = nullptr) { \
	static SimpleParser::Type* _static_type = nullptr; \
	if (!_static_type) { \
		if (!newType) {                                              \
            SimpleParser::Type* parent = _Inherit::staticType();     \
            newType = SimpleParser::Type::Register(#_MeClass, _MeClass::createType); \
            newType->setParent(parent);                              \
            _static_type = newType;                                  \
            _MeClass::initializeType(_static_type);                  \
        }  \
	} \
	return _static_type; \
} \
static void _register_ ## _MeClass() { _MeClass::staticType(); } \
 \
static Component* createType(std::string name, std::string type) \
{ \
	Component* ret = (Component*)new _MeClass(name, type); \
	return ret; \
} \
 \
private: \

#define MECLASS_VIRTUAL(_MeClass, _Inherit)  \
	public: \
static SimpleParser::Type* staticType(SimpleParser::Type* newType = nullptr) { \
	static SimpleParser::Type* _static_type = nullptr; \
	if (!_static_type) { \
		if (!newType) {                                              \
            SimpleParser::Type* parent = _Inherit::staticType();     \
            newType = SimpleParser::Type::Register(#_MeClass, _MeClass::createType); \
            newType->setParent(parent);                              \
            _static_type = newType;                                  \
            _MeClass::initializeType(_static_type);                  \
        }  \
	} \
	return _static_type; \
} \
static void _register_ ## _MeClass() { _MeClass::staticType(); } \
 \
private: \

#endif // !SIMPLEPARSER_TYPE_H
