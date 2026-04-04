#include "Rhea/Unit.h"
#include "SimpleParser/Method.h"

using SimpleParser::Type;
using SimpleParser::Method;

namespace rhea
{
	void Unit::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		m_app = app;
		m_attributes = attributes;
		m_values = values;
	}

	Unit::Unit(const Unit& u) : Component(u)
	{
		m_path = u.m_pathname;
		m_parent = nullptr;
		m_type = u.m_type;
		m_app = u.m_app;
		m_refcount = 0;
		m_attributes = u.m_attributes;
		m_values = u.m_values;
		// copy the children
		for (auto it = u.m_ordered_children.begin(); it != u.m_ordered_children.end(); it++)
		{
			Component* child = (*it)->Clone();
			addChild(child);
		}
	}

	void Unit::addChild(Component* child)
	{		
		Unit* u_child = doCast<Unit>(child);
		if (u_child)
			u_child->m_path = m_path + "/" + child->getPathName();
		child->setParent(this);
		m_children[child->getPathName()] = child;
		m_ordered_children.push_back(child);
		if (u_child)
			u_child->onParented();
	}

	void Unit::removeChild(Component* child)
	{		
		m_children.erase(child->getPathName());
		m_ordered_children.erase(std::remove(m_ordered_children.begin(), m_ordered_children.end(), child));
		child->setParent(nullptr);
		Unit* u_child = doCast<Unit>(child);
		if (u_child)
			u_child->onRemoved();
	}

	void Unit::addComponent(Component* component)
	{
		if (!component) return;

		// Traverse the type hierarchy and register the component under each type
		SimpleParser::Type* type = component->getType();
		while (type) {
			m_unique_components[type] = component;
			type = type->getParent();
		}
		// add as child
		addChild(component);
	}

	void Unit::removeComponent(Component* component)
	{
		if (!component) return;

		// first remove child
		removeChild(component);
		
		// remove from map, but only if the stored pointer == component
		SimpleParser::Type* type = component->getType();
		while (type) {
			auto it = m_unique_components.find(type);
			if (it != m_unique_components.end() && it->second == component) {
				m_unique_components.erase(it);
			}
			type = type->getParent();
		}
	}
	
	Component* Unit::find(std::string path, bool create)
	{
		// get first dir
		std::string subpath;
		size_t pos = path.find("/");
		if (pos == std::string::npos) {
			// no further subpath 
			subpath = path;
			auto unit_it = m_children.find(path);
			if (unit_it == m_children.end()) {
				if (create) {
					// not found - create a unit as container
					Unit* u = new Unit(path, "//core/types/unit");
					addChild(u);
					return u;
				} 
				else {
					return nullptr;
				}
			}
			else
				return unit_it->second;
		}
		else {
			if (!pos && !m_parent) {
				// handle special case for root
				if (path.substr(0, 2) == "//") {					
					path = path.substr(2);
					pos = path.find("/");
					if (pos == std::string::npos) {
						// no further subpath 
						subpath = path;
						auto unit_it = m_children.find(path);
						if (unit_it == m_children.end())
						{
							if (create) {
								// not found - create a unit as container
								Unit* u = new Unit(path, "//core/types/unit");
								addChild(u);
								return u;
							}
							else {
								return nullptr;
							}
						}
						return unit_it->second;
					}
				}
			}
			// check children
			subpath = path.substr(0, pos);
			auto unit_it = m_children.find(subpath);
			if (unit_it == m_children.end()) {
				if (create) {
					// not found - create a unit as container
					Unit* u = new Unit(subpath, "//core/types/unit");
					addChild(u);
					unit_it = m_children.find(subpath);
				}
				else {			
					return nullptr;
				}
			}
			// if this not a unit return
			Unit* unit = doCast<Unit>(unit_it->second);
			if (!unit)
				return nullptr;
			else
				return unit->find(path.substr(pos + 1, path.length()), create);
		}
	}

	Component* Unit::find(std::string path, const std::string& basepath, bool create)
	{
		if (path == "")
			return nullptr;
		if (path.size() > 1)
			if (path[0] == '/') {
				if (path[1] != '/') {
					// use parent of basepath
					size_t pos = basepath.rfind("/");
					if (pos != std::string::npos && pos > 1)
						path = basepath.substr(0, pos) + path;
				}
				// else is absolute
			}
			else
				path = basepath + "/" + path;
		return find(path, create);
	}

	Unit* Unit::findByType(std::string type_name)
	{
		// myself?
		if (m_type->getName() == type_name)
			return this;

		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Unit* u = doCast<Unit>(it->second);
			if (u) {
				Unit* ret = u->findByType(type_name);
				if (ret)
					return ret;
			}
		}

		// not found
		return nullptr;
	}

	std::vector<Component*> Unit::getAllComponents() const
	{
		std::vector<Component*> components;
		for (const auto& it : m_unique_components) {
			Component* component = it.second;
			if (std::find(components.begin(), components.end(), component) == components.end()) {
				components.push_back(component);
			}
		}
		return components;
	}	

	std::string Unit::getPath(std::string& unit_path)
	{
		// is the string a full path?
		if (unit_path.length() > 1 && unit_path[0] == '/' && unit_path[1] == '/')
			return unit_path;
		else if (m_attributes.find("basepath") != m_attributes.end())
			return m_attributes["basepath"] + "/" + unit_path;
		else
			return "//" + unit_path;
	}

	Component* Unit::getChild(std::string name)
	{
		auto it = m_children.find(name);
		if (it != m_children.end()) {
			return it->second;
		}
		return nullptr;
	}

	void Unit::setName(std::string name)
	{
		m_path = name;
		Component::setName(name);
	}
	
	Variant Unit::getValue(std::string name) const
	{
		if (name != "") {
			auto val = m_values.find(name);
			if (val != m_values.end()) {
				return val->second;
			}
		}
		return Variant();
	}

	std::string Unit::getValueString(std::string name, std::string default_val) const
	{
		auto val = m_values.find(name);
		if (val != m_values.end()) {
			return val->second.toString();
		}
		return default_val;
	}

	std::string Unit::getValueString(std::string name) const
	{
		if (name != "") {
			auto val = m_values.find(name);
			if (val != m_values.end()) {
				return val->second.toString();
			}
		}
		return std::string("");
	}

	Variant Unit::getValue(std::string name, Variant default_val) const
	{
		auto val = m_values.find(name);
		if (val != m_values.end()) {
			return val->second;
		}
		return default_val;
	}

	void Unit::setValue(std::string name, Variant val)
	{
		m_values[name] = val;
	}

	std::vector<Unit::Export> Unit::getReferences()
	{
		return std::vector<Export>();
	}

	void Unit::ready()
	{
		// call ready for all children
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			Unit* unit = doCast<Unit>(it->second);
			if (unit)
				unit->ready();
		}
	}

	void Unit::broadcast(std::string method_name)
	{
		// do i have this method?
		message(method_name);
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Unit* c = doCast<Unit>(it->second);
			if (c)
				c->broadcast(method_name);
		}
	}

	void Unit::message(std::string method_name)
	{
		if (method_name == "logme") logMe(); 		
	}

	void Unit::registerEventHandler(std::string event_name, void* owner, Event::EventHandler handler)
	{
		m_eventhandlers[event_name][owner] = handler;
	}

	void Unit::unregisterEventHandler(std::string event_name, void* owner)
	{
		auto it = m_eventhandlers.find(event_name);
		if (it != m_eventhandlers.end()) {
			(it->second)[owner] = nullptr;				
		}
	}

	void Unit::registerEventHandlerChildren(std::string event_name, void* owner, Event::EventHandler handler)
	{
		registerEventHandler(event_name, owner, handler);
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Unit* c = doCast<Unit>(it->second);
			if (c)
				c->registerEventHandlerChildren(event_name, owner, handler);
		}
	}

	void Unit::sendEvent(std::string event_name, Event* event)
	{
		auto it = m_eventhandlers.find(event_name);
		if (it != m_eventhandlers.end()) {
			auto handlers_copy = it->second;  // Copy the handlers so |this| may be destroyed
			for (auto& eh : handlers_copy) {
				if (eh.second)
					eh.second(event, this);  // |this| might be destroyed here
			}
		}
	}

	void Unit::sendEventChildren(std::string event_name, Event* event)
	{
		sendEvent(event_name, event);
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Unit* c = doCast<Unit>(it->second);
			if (c)
				c->sendEventChildren(event_name, event);
		}
	}

	void Unit::sendEventSubtree(Unit* top, std::string event_name, Event* event)
	{
		if (top == this)
			return;

		Unit* const parent = m_parent;

		if (parent && parent != top)
			parent->sendEventSubtree(top, event_name, event);

		sendEvent(event_name, event);
	}

	void Unit::bubbleEvent(std::string event_name, Event* event)
	{
		sendEvent(event_name, event);
		if (m_parent && (!event || !event->isPropagationStoppped()))
			m_parent->bubbleEvent(event_name, event);
	}

	void Unit::traverse(std::function<bool(std::string&, Component*)> handler)
	{
		if (handler(this->m_pathname, this)) {
			for (auto it = m_children.begin(); it != m_children.end(); ++it) {
				std::string name = it->first;
				Unit* unit_child = doCast<Unit>(it->second);
				if (unit_child)
					unit_child->traverse(handler);
				else
					handler(it->second->getPathName(), it->second);
			}
		}
	}

	void Unit::children(std::function<bool(std::string&, Component*)> handler)
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			std::string name = it->first;
			if (!handler(name, it->second))
				return;
		}
	}


	Unit::~Unit()
	{
		// iterate over children and delete them
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			delete it->second;
		}
	}

	void Unit::logMe()
	{
		printf("--------\nName: %s\nPath: %s\nType: %s\n", m_pathname.c_str(), m_path.c_str(), m_type->getName().c_str());
	}
}