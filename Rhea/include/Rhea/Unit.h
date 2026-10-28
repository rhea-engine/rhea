/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_UNIT_H
#define RHEA_UNIT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

#include "SimpleParser/Variant.h"
#include "SimpleParser/Type.h"
#include "Event.h"
#include "Element.h"
#include "Error.h"
#include "Rhea/Component.h"

using simpleparser::Variant;

namespace rhea
{
	class Application;
	class Visitor;

	class Unit : public Component
	{
		MECLASS(Unit, Component)
	public:
		Unit(std::string path_name, std::string type_name) :
			m_path(path_name),
			m_refcount(0),
			Component(path_name, type_name)
		{}		
		virtual ~Unit();

		void onDestroy() override;
		void detachFromParent();

		// async prepare
		typedef std::function<void(Error*)> ReadyHandler;

		// ready signal - all dependant units are prepared and we now finalize for usage, recursive to children
		void ready() override;

		// add a child resource
		// returns false if the a child with the same name already exists
		// if silent is true no log is generated
		bool addChild(Component* child, bool silent = false);

		// remove a child
		// returns false if the child was not found or the child was not a child of this unit
		bool removeChild(Component* child);

		// check if a component can be removed
		virtual bool canRemoveComponent(const Component* component) const { return true; }

		// add component
		// a component is a child which is stored in the type lookup table
		// this is allows lookup by type
		virtual void addComponent(Component* component);

		// create and add component
		template <typename T, typename... Args>
		T* addComponentUnique(Args&&... args) {
			T* component = new T(std::forward<Args>(args)...);
			Component* old_component = getComponent<T>();
			if (old_component) {
				removeComponent(old_component);
				delete old_component;
			}
			// Register the component under its exact type and base types
			addComponent(component);
			return component;
		}

		// remove a component
		virtual void removeComponent(Component* component);

		template <typename T>
		void deleteComponent() {
			Component* old_component = getComponent<T>();
			if (old_component) {
				removeComponent(old_component);
				delete old_component;
			}
		}		

		// get a component with the given type - which may be a base type
		template <typename T>
		T* getComponent() {
			auto it = m_unique_components.find(T::staticType());
			return it != m_unique_components.end() ? static_cast<T*>(it->second) : nullptr;
		}					

		std::vector<Component*> getAllComponents() const;

		// ensure a component of the given type exists - if not create it with the given fallback name
		template <typename T>
		T* ensureComponent(const std::string& fallback_name)
		{
			if (auto* existing = getComponent<T>())
				return existing;

			for (auto* child : m_ordered_children) {
				if (auto* component = doCast<T>(child)) {
					addComponent(component);
					return component;
				}
			}

			return addComponentUnique<T>(fallback_name);
		}

		// get full path
		const std::string& getPath() const noexcept { return m_path; }

		// get full path for other unit
		std::string getPath(const std::string& unit_path) const;

		// get basepath of unit
		const std::string getBasePath() const { auto it = m_attributes.find("basepath"); return it != m_attributes.end() ? it->second : std::string{}; }

		// find child with given name
		Component* getChild(std::string name);
		Component* getChild(std::string name) const;

		// find by path
		Component* find(std::string path, bool create = false);
		Component* find(std::string path, const std::string& basepath, bool create);

		void setName(std::string name) override;

		// find nearest by type
		template <class T>
		T* findByType() {
			if (doCast<T>(this)) return (T*)this;
			for (auto it = m_children.begin(); it != m_children.end(); ++it) {
				Unit* child = doCast<Unit>(it->second);
				if (child) {
					T* ret = child->findByType<T>();
					if (ret)
						return ret;
				}
			}
			return nullptr;
		}
		Unit* findByType(std::string type_name);
		Component* findByName(const std::string& name);

		// find parent by type
		template <class T>
		T* findParentByType() {
			Unit* u = m_parent;
			while (u) {
				if (doCast<T>(u)) return (T*)u;
				u = u->m_parent;
			}
			return nullptr;
		}

		// events
	
		// register the event handler for event event_name
		void registerEventHandler(std::string event_name, void* owner, Event::EventHandler handler);

		// unregister a handler
		void unregisterEventHandler(std::string event_name, void* owner);
		
		// register event handler in all children of the unit
		void registerEventHandlerChildren(std::string event_name, void* owner, Event::EventHandler handler);

		// check if an event-handler is registered for an event and an owner
		bool hasEventHandler(const std::string& event_name, void* owner) const {
			auto it = m_eventhandlers.find(event_name);
			if (it == m_eventhandlers.end()) return false;
			return it->second.find(owner) != it->second.end();
		}

		// send Event
		void sendEvent(std::string event_name, Event* event);

		// send Event to all children
		void sendEventChildren(std::string event_name, Event* event);

		// send event to parent subtree
		void sendEventSubtree(Unit* top, std::string event_name, Event* event);

		// bubble event (send to parents)
		void bubbleEvent(std::string event_name, Event* event);

		// traverse subtree
		// if the handler returns false the children will not be called
		void traverse(std::function<bool(std::string&, Component*)> handler);

		// execute handler on all children
		// if the handler return false the traverse is stopped
		void children(std::function<bool(std::string&, Component*)> handler, bool ordered = false);

		// increment reference count
		void incRef() { m_refcount++; }

		// decrease reference count
		void decRef() { m_refcount--; }

		// Clone method, returns a pointer to a copy of the object
		Unit* Clone() const override { return new Unit(*this); }
		
		// instance this unit - this will create a copy of the unit and all its children
		Component* instanceCreate(const std::string& name, Component* source, InstantiationContext& context);
		void instanceSetup(Component* source, Component* target, InstantiationContext& context);

		// print my info
		virtual void logMe();
		
	protected:		
		std::string m_path;		
		void updatePath();

		// the children of the unit - this may be components
		std::unordered_map<std::string, Component*> m_children;
		// the children in the order of when they whre added
		std::vector<Component*> m_ordered_children;
		// the types of the children if added as component - per type only one component
		std::unordered_map<simpleparser::Type*, Component*> m_unique_components;

		// event handlers for this unit
		std::unordered_map < std::string, std::unordered_map<void*, Event::EventHandler >> m_eventhandlers;
		int m_refcount;		

		// copy constructor
		Unit(const Unit&);
	};
}

#endif // !RHEA_UNIT_H
