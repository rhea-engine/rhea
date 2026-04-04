#ifndef RHEA_COMPONENT_H
#define RHEA_COMPONENT_H

#include <SimpleParser/Type.h>
#include <SimpleParser/Variant.h>

namespace rhea
{
    class Unit;

    class Component : public SimpleParser::Variant {
       
    protected:
        Unit* m_parent = nullptr;  // Pointer to the parent Unit
        std::string m_pathname;

    public:
        Component(std::string name, std::string type_name) :
            m_pathname(name),
            m_parent(nullptr),
            SimpleParser::Variant(SimpleParser::Type::get(type_name))
        {}
        Component(std::string name) :
            Component(name, "//core/types/component")
        {}
        // Copy constructor
        Component(const Component& other) : 
            m_parent(nullptr),
            m_pathname(other.m_pathname)
        {} 
        virtual ~Component() = default;

        // Set and get parent
        void setParent(Unit* parent) { m_parent = parent; }
        Unit* getParent() const { return m_parent; }

        // get name
        std::string& getPathName() { return m_pathname; }
        
        // rename this component
        // name: the new name
        // note: 
        // this removes the component from its parent and readds it under it new name
        virtual void setName(std::string name);

        // Clone method, returns a pointer to a copy of the object
        virtual Component* Clone() const { return new Component(*this); }

        // tree/parent functions

        // get common ancestor
        Component* getCommonAncestor(Component* u2);

        // is unit the ancestor
        bool isAncestor(Component* u2);

        // type functions

        // create unit type
        static Component* createType(std::string name, std::string type_name);

        // type initialization
        static void initializeType(SimpleParser::Type* type);

        // type helper
        static SimpleParser::Type* staticType(SimpleParser::Type* newType = nullptr) {
            static SimpleParser::Type* _static_type = nullptr;
            if (!_static_type) {
                if (!newType) {

                    SimpleParser::Type* parent = nullptr;
                    newType = SimpleParser::Type::Register("Component", Component::createType);
                    newType->setParent(parent);
                    _static_type = newType;
                    Component::initializeType(_static_type);
                }
            }
            return _static_type;
        }
    };
}
#endif // !RHEA_COMPONENT_H
