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
#include <Rhea/Ref.h>

namespace rhea
{
    class Application;
    class Unit;
    struct ResourceContext;

    class Component : public simpleparser::Variant
    {

    public:
        enum class ComponentOrigin
        {
            Configured,
            Runtime
        };

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

        // ready signal - finalize the component for usage after load
        virtual void ready() {}

		// activate and deactivate the component
		virtual void onActivate() {}
		virtual void onDeactivate() {}

        // Set and get parent
        void setParent(Unit* parent) { m_parent = parent; }
        Unit* getParent() const { return m_parent; }

        // handle on parented
        virtual void onParented() {}
        // onRemove handler - called when the unit is removed from its parent
        virtual void onRemoved() {}

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
		//  resolve references in values - replaces any value that is a reference with the actual value
        virtual void resolveReferences(const std::string& context_root, const std::string& source_root);

        // get referenced resources - returns a list of resources that are referenced by this component
        enum class ResourceUsage {
            Load,           // only load
			Prepare,        // prepare for usage
            EnsureReady,    // ensure ready
            Instantiate     // instantiate
        };
        struct ResourceReference {
            std::string name;
            std::string type;
            simpleparser::Variant value;
            ResourceUsage usage = ResourceUsage::Load;
        };
        virtual std::vector<ResourceReference> getReferences();

        // Get attributes map
        const std::unordered_map<std::string, std::string>& getAttributes() const {
            return m_attributes;
        }

        // Get attribute by key
        std::string getAttribute(const std::string& key) const {
            auto it = m_attributes.find(key);
            return it != m_attributes.end() ? it->second : "";
        }
		void setAttribute(const std::string& key, const std::string& value) {
			m_attributes[key] = value;
		}

        // Get values map
        const std::unordered_map<std::string, simpleparser::Variant>& getValues() const {
            return m_values;
        }


        // Clone method, returns a pointer to a copy of the object
        virtual Component* Clone() const { return new Component(*this); }
        // may instantiated?
		virtual bool isInstanceable() const { return true; }
        virtual Component* instance(const std::string& name = "");
		// use two phases for instancing - creation and setup
        struct InstantiationContext {
			std::unordered_map<const Component*, Component*> instantiated_components;
        };
        virtual Component* instanceCreate(const std::string& name, Component* source, InstantiationContext& context);
		virtual void instanceSetup(Component* source, Component* target, InstantiationContext& context);

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

		ComponentOrigin getOrigin() const { return m_origin; }
		void setOrigin(ComponentOrigin origin) { m_origin = origin; }

    protected:
        Application* m_app;
        Unit* m_parent = nullptr;  // Pointer to the parent Unit
        std::string m_pathname;
        bool m_destroying = false; // Flag to indicate if the component is being destroyed

        std::unordered_map<std::string, std::string> m_attributes;
        std::unordered_map<std::string, simpleparser::Variant> m_values;

        // runtime or configuration origin?
		// used to determine if the component should be instanced or not
		ComponentOrigin m_origin = ComponentOrigin::Runtime;

		// resource property metadata
        struct ResourceProperty {
            std::string name;
            std::string resource_type;
            ResourceUsage usage;

            std::function<std::string(const Component&)> get_id;
            std::function<void(Component&, std::string)> set_id;
            std::function<void(Component&, Component&)> copy;
            std::function<void(Component&, const std::string&)> set_base_path;
        };

        template<class Owner, class Target>
        static void registerResourceProperty(
            simpleparser::Type* type,
            std::string name,
            Ref<Target> Owner::* member,
            std::string resource_type,
            ResourceUsage usage)
        {
            s_resource_properties[type].push_back({
                std::move(name),
                std::move(resource_type),
                usage,
                [member](const Component& component) {
                    return (static_cast<const Owner&>(component).*member).id();
                },
                [member](Component& component, std::string id) {
                    (static_cast<Owner&>(component).*member).setId(std::move(id));
                },
                [member](Component& source, Component& target) {
                    (static_cast<Owner&>(target).*member) =
                    (static_cast<Owner&>(source).*member);
                },
                [member](Component& component, const std::string& base_path) {
                    (static_cast<Owner&>(component).*member).setBasePath(base_path);
                }
                });
        }       
        static void copyResourceProperties(Component& source, Component& target);

        std::string normalizeReferenceId(
            const std::string& id,
            const ResourceContext& context) const;

        template<class T>
        void normalizeReference(
            Ref<T>& reference,
            const ResourceContext& context) const
        {
            const std::string base_path = getAttribute("basepath");
            if (!base_path.empty())
                reference.setBasePath(base_path);

            reference.setId(normalizeReferenceId(reference.id(), context));
        }

        static std::unordered_map<simpleparser::Type*, std::vector<ResourceProperty>> s_resource_properties;
    };
}
#endif // !RHEA_COMPONENT_H
