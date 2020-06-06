#include "Unit.h"

namespace Rhea
{
	void Unit::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		m_app = app;
		m_attributes = attributes;
		m_values = values;
	}

	void Unit::addChild(Unit* child)
	{		
		child->m_path = m_path + "/" + child->m_pathname;
		child->m_parent = this;
		m_children[child->m_pathname] = child;
	}
	
	Unit* Unit::find(std::string path, bool create)
	{
		// get first dir
		std::string subpath;
		size_t pos = path.find("/");
		if (pos == std::string::npos) {
			// no further subpath 
			subpath = path;
			auto unit_it = m_children.find(path);
			if (unit_it == m_children.end())
				// TODO if create
				return NULL;
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
							// TODO if create
							return NULL;
						return unit_it->second;
					}
				}
			}
			subpath = path.substr(0, pos);
			auto unit_it = m_children.find(subpath);
			if (unit_it == m_children.end())
				// TODO if create
				return NULL;
			return unit_it->second->find(path.substr(pos + 1, path.length()), create);
		}
	}

	Unit* Unit::findByType(std::string type_name)
	{
		// myself?
		if (m_type_name == type_name)
			return this;

		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Unit* ret = it->second->findByType(type_name);
			if (ret)
				return ret;
		}

		// not found
		return nullptr;
	}

	Variant Unit::getValue(std::string name)
	{

		auto val = m_values.find(name);
		if (val != m_values.end()) {
			return val->second;
		}
		return Variant();
	}

	Variant Unit::getValue(std::string name, Variant default_val)
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

	void Unit::broadcast(std::string method_name)
	{
		// do i have this method?
		message(method_name);
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Unit* c = it->second;
			c->broadcast(method_name);
		}
	}

	void Unit::message(std::string method_name)
	{
		if (method_name == "logme") logMe(); 		
	}

	void Unit::registerEventHandler(std::string event_name, Event::EventHandler handler)
	{
		m_eventhandlers[event_name].push_back(handler);
	}

	void Unit::registerEventHandlerChildren(std::string event_name, Event::EventHandler handler)
	{
		registerEventHandler(event_name, handler);
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Unit* c = it->second;
			c->registerEventHandlerChildren(event_name, handler);
		}
	}

	void Unit::sendEvent(std::string event_name, Event* event)
	{
		auto it = m_eventhandlers.find(event_name);
		if (it != m_eventhandlers.end()) {
			for (auto eh_it = it->second.begin(); eh_it != it->second.end(); ++eh_it) {
				Event::EventHandler eh = *eh_it;
				eh(event, this);
			}
		}
	}

	void Unit::sendEventChildren(std::string event_name, Event* event)
	{
		sendEvent(event_name, event);
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Unit* c = it->second;
			c->sendEventChildren(event_name, event);
		}
	}

	void Unit::addElement(std::string name, Element* element)
	{
		Element* previous = getElement(name);
		if (previous)
			delete previous;
		m_elements[name] = element;
	}

	Element* Unit::getElement(std::string name)
	{
		auto it = m_elements.find(name);
		if (it != m_elements.end())
			return it->second;
		else
			return nullptr;
	}

	void Unit::logMe()
	{
		printf("--------\nName: %s\nPath: %s\nType: %s\n", m_pathname.c_str(), m_path.c_str(), m_type_name.c_str());
	}
}