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

using SimpleParser::Variant;

namespace rhea
{
	class Application;
	class Visitor;

	class Unit : public Component
	{
		MECLASS(Unit, Component)
	public:
		Unit(std::string path_name, std::string type_name) :
			m_app(NULL),
			m_path(path_name),
			m_refcount(0),
			Component(path_name, type_name)
		{}		
		virtual ~Unit();
		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		virtual void setup(Application* app) { m_app = app; }

		// async prepare
		typedef std::function<void(Error*)> ReadyHandler;

		// ready signal - all dependant units are prepared and we now finalize for usage, recursive to children
		virtual void ready();

		// add a child resource
		void addChild(Component* child);

		// remove a child
		void removeChild(Component* child);

		// handle on parented
		virtual void onParented() {}
		virtual void onRemoved() {}

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

		// get full path
		std::string getPath() { return m_path; }

		// get full path for other unit
		std::string getPath(std::string& unit_path);

		// get basepath of unit
		std::string getBasePath() { return m_attributes["basepath"]; }

		// find child with given name
		Component* getChild(std::string name);

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

		// get value
		Variant getValue(std::string name) const;
		// get value with default
		Variant getValue(std::string name, Variant default_val) const;
		// get a value as string
		std::string getValueString(std::string name) const;
		std::string getValueString(std::string name, std::string default_val) const;
		// set a value
		void setValue(std::string name, Variant val);
		// get exported values
		typedef struct {
			std::string name;
			std::string type;
			Variant value;
		} Export;
		virtual std::vector<Export> getReferences();
		
		// messaging and events

		// broadcast message
		void broadcast(std::string method_name);
		// message implementation
		virtual void message(std::string method_name);

		// register the event handler for event event_name
		void registerEventHandler(std::string event_name, void* owner, Event::EventHandler handler);

		// unregister a handler
		void unregisterEventHandler(std::string event_name, void* owner);
		
		// register event handler in all children of the unit
		void registerEventHandlerChildren(std::string event_name, void* owner, Event::EventHandler handler);

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
		void children(std::function<bool(std::string&, Component*)> handler);

		// set Application
		void setApplication(Application* app) { m_app = app; }

		// get Application
		Application* getApplication() { return m_app; }

		// increment reference count
		void incRef() { m_refcount++; }

		// decrease reference count
		void decRef() { m_refcount--; }

		// Clone method, returns a pointer to a copy of the object
		Unit* Clone() const override { return new Unit(*this); }

		// print my info
		virtual void logMe();
	protected:
		Application* m_app;
		std::string m_path;
		std::unordered_map<std::string, std::string> m_attributes;
		std::unordered_map<std::string, Variant> m_values;

		// the children of the unit - this may be components
		std::unordered_map<std::string, Component*> m_children;
		// the children in the order of when they whre added
		std::vector<Component*> m_ordered_children;
		// the types of the children if added as component - per type only one component
		std::unordered_map<SimpleParser::Type*, Component*> m_unique_components;

		// event handlers for this unit
		std::unordered_map < std::string, std::unordered_map<void*, Event::EventHandler >> m_eventhandlers;
		int m_refcount;

		// copy constructor
		Unit(const Unit&);
	};
}

#endif // !RHEA_UNIT_H
