#ifndef RHEA_UNIT_H
#define RHEA_UNIT_H

#include <string>
#include <vector>
#include <unordered_map>

#include "SimpleParser/Variant.h"
#include "SimpleParser/Type.h"
#include "Event.h"
#include "Element.h"
#include "Error.h"

using SimpleParser::Variant;

namespace Rhea
{
	class Application;
	class Visitor;

	class Unit : public SimpleParser::Variant
	{
	public:
		Unit(std::string path_name, std::string type_name) :
			m_app(NULL),
			m_pathname(path_name),
			m_parent(NULL),
			m_path(path_name),
			m_refcount(0),
			SimpleParser::Variant(SimpleParser::Type::get(type_name))
		{}		
		virtual ~Unit();
		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		virtual void setup(Application* app) { m_app = app; }

		// async prepare
		typedef std::function<void(Error*)> ReadyHandler;
		// async prepare
		virtual void prepare(ReadyHandler handler);

		// ready signal - all dependant units are prepared and we now finalize for usage, recursive to children
		virtual void ready();

		// add a child resource
		void addChild(Unit* child);

		// remove a child
		void removeChild(Unit* child);

		// get parent - returns NULL for top
		Unit* getParent() { return m_parent; }

		// get name
		std::string getPathName() { return m_pathname; }

		// get full path
		std::string getPath() { return m_path; }

		// get full path for other unit
		std::string getPath(std::string& unit_path);

		// find child with given name
		Unit* getChild(std::string name);

		// find by path
		Unit* find(std::string path, bool create = false);

		// find nearest by type
		Unit* findByType(std::string type_name);

		// get common ancestor
		Unit* getCommonAncestor(Unit* u2);

		// is unit the ancestor
		bool isAncestor(Unit* u2);

		// get value
		Variant getValue(std::string name);
		// get value with default
		Variant getValue(std::string name, Variant default_val);
		// set a value
		void setValue(std::string name, Variant val);

		// rename
		void setName(std::string name);

		// Elements

		// add an element with the given name
		void addElement(std::string name, Element* element);

		// get the element with the given name
		Element* getElement(std::string name);

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
		void traverse(std::function<bool(std::string&, Unit*)> handler);

		// execute handler on all children
		// if the handler return false the traverse is stopped
		void children(std::function<bool(std::string&, Unit*)> handler);

		// get Application
		Application* getApplication() { return m_app; }

		// increment reference count
		void incRef() { m_refcount++; }

		// decrease reference count
		void decRef() { m_refcount--; }

		
		// type functions
		
		// create unit type
		static Unit* createType(std::string name, std::string type_name);

		// type initialization
		static void initializeType(SimpleParser::Type* type);

		// print my info
		virtual void logMe();
	protected:
		Application* m_app;
		Unit* m_parent;
		std::string m_pathname;
		std::string m_path;
		std::unordered_map<std::string, std::string> m_attributes;
		std::unordered_map<std::string, Variant> m_values;
		std::unordered_map<std::string, Unit*> m_children;
		std::unordered_map < std::string, std::unordered_map<void*, Event::EventHandler >> m_eventhandlers;
		std::unordered_map<std::string, Element*> m_elements;
		int m_refcount;
	};
}

#endif // !RHEA_UNIT_H
