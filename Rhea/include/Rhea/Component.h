/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_COMPONENT_H
#define RHEA_COMPONENT_H

#include <string>
#include <unordered_map>
#include <vector>

#include <SimpleParser/Type.h>
#include <SimpleParser/Variant.h>

namespace rhea
{
    class Application;
    class Unit;

    class Component : public simpleparser::Variant
    {

    public:
        Component(std::string name, std::string type_name) :
			m_app(nullptr),
            m_pathname(name),
            m_parent(nullptr),
            simpleparser::Variant(simpleparser::Type::get(type_name))
        {}
        Component(std::string name) :
            Component(name, "//core/types/component")
        {}
        // Copy constructor
        Component(const Component& other) :
            m_app(other.m_app),
            m_parent(nullptr),
            m_pathname(other.m_pathname),
            m_attributes(other.m_attributes),
            m_values(other.m_values)
        {}
        virtual ~Component() = default;

        void destroy();
        virtual void onDestroy() {}

        virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values);
        virtual void setup(Application* app) { m_app = app; }

        // ready signal - finalize the component for usage
        virtual void ready() {}

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

        // get value
        simpleparser::Variant getValue(std::string name) const;
        // get value with default
        simpleparser::Variant getValue(std::string name, simpleparser::Variant default_val) const;
        // get a value as string
        std::string getValueString(std::string name) const;
        std::string getValueString(std::string name, std::string default_val) const;
        // set a value
        void setValue(std::string name, simpleparser::Variant val);
        // get exported values
        typedef struct {
            std::string name;
            std::string type;
            simpleparser::Variant value;
        } Export;
        virtual std::vector<Export> getReferences();


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
        static void initializeType(simpleparser::Type* type);

        // type helper
        static simpleparser::Type* staticType(simpleparser::Type* newType = nullptr) {
            static simpleparser::Type* _static_type = nullptr;
            if (!_static_type) {
                if (!newType) {

                    simpleparser::Type* parent = nullptr;
                    newType = simpleparser::Type::Register("Component", Component::createType);
                    newType->setParent(parent);
                    _static_type = newType;
                    Component::initializeType(_static_type);
                }
            }
            return _static_type;
        }

        // set Application
        void setApplication(Application* app) { m_app = app; }

        // get Application
        Application* getApplication() { return m_app; }

    protected:
        Application* m_app;
        Unit* m_parent = nullptr;  // Pointer to the parent Unit
        std::string m_pathname;
        bool m_destroying = false; // Flag to indicate if the component is being destroyed

        std::unordered_map<std::string, std::string> m_attributes;
        std::unordered_map<std::string, simpleparser::Variant> m_values;
    };
}
#endif // !RHEA_COMPONENT_H
