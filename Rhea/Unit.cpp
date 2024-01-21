#include "Unit.h"
#include "SimpleParser/Method.h"

using SimpleParser::Type;
using SimpleParser::Method;

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
		m_ordered_children.push_back(child);
		child->onParented();
	}

	void Unit::removeChild(Unit* child)
	{		
		m_children.erase(child->m_pathname);
		m_ordered_children.erase(std::remove(m_ordered_children.begin(), m_ordered_children.end(), child));
		child->m_parent = nullptr;
		child->onRemoved();
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
					return NULL;
				}
			}
			return unit_it->second->find(path.substr(pos + 1, path.length()), create);
		}
	}

	Unit* Unit::find(std::string path, std::string basepath, bool create)
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
			Unit* ret = it->second->findByType(type_name);
			if (ret)
				return ret;
		}

		// not found
		return nullptr;
	}

	Unit* Unit::getCommonAncestor(Unit* u2)
	{
		if (!u2)
			return nullptr;

		Unit* u1 = this;

		if (u1 == u2)
			return u1;

		// if u1 is already anchestor u2 is the result
		if (u1 && u1->isAncestor(u2))
			return u2;

		while (u1)
		{
			if (u2->isAncestor(u1))
				break;

			u1 = u1->m_parent;
		}

		return u1;
	}

	bool Unit::isAncestor(Unit* u2)
	{
		// if we have no parent we have no ancestor
		if (!m_parent)
			return false;

		// check our immediate parent
		if (m_parent == u2)
			return true;

		// not our parent - check back up the tree
		return m_parent->isAncestor(u2);
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

	Unit* Unit::getChild(std::string name)
	{
		auto it = m_children.find(name);
		if (it != m_children.end()) {
			return it->second;
		}
		return nullptr;
	}

	void Unit::setName(std::string name)
	{
		// update the full path
		if (m_parent) {
			m_path = m_parent->getPath() + "/" + name;
			m_parent->m_children[name] = this;
			m_parent->m_children.erase(m_pathname);
		}
		else {
			m_path = name;
		}

		// update my unit name
		m_pathname = name;
	}

	Variant Unit::getValue(std::string name)
	{
		if (name != "") {
			auto val = m_values.find(name);
			if (val != m_values.end()) {
				return val->second;
			}
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

	std::vector<Unit::Export> Unit::getReferences()
	{
		return std::vector<Export>();
	}

	void Unit::ready()
	{
		// call ready for all children
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			Unit* unit = it->second;
			unit->ready();
		}
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
			Unit* c = it->second;
			c->registerEventHandlerChildren(event_name, owner, handler);
		}
	}

	void Unit::sendEvent(std::string event_name, Event* event)
	{
		auto it = m_eventhandlers.find(event_name);
		if (it != m_eventhandlers.end()) {
			for (auto eh_it = it->second.begin(); eh_it != it->second.end(); ++eh_it) {
				Event::EventHandler eh = eh_it->second;
				if (eh)
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
		if (!event->isPropagationStoppped() && m_parent)
			m_parent->bubbleEvent(event_name, event);
	}

	void Unit::traverse(std::function<bool(std::string&, Unit*)> handler)
	{
		if (handler(this->m_pathname, this)) {
			for (auto it = m_children.begin(); it != m_children.end(); ++it) {
				std::string name = it->first;
				//handler(name, it->second);
				it->second->traverse(handler);
			}
		}
	}

	void Unit::children(std::function<bool(std::string&, Unit*)> handler)
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			std::string name = it->first;
			if (!handler(name, it->second))
				return;
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

	Unit* Unit::createType(std::string name, std::string type_name)
	{
		Unit* ret = new Unit(name, type_name);
		return ret;
	}

	void Unit::initializeType(Type* type)
	{
		type->registerMethod("setName", &Unit::setName);
		type->registerMethod("getName", &Unit::getPathName);

		type->registerProperty("path", Type::get(Type::BaseType::STRING), "setName", "getName");
	}

	Unit::~Unit()
	{
		// iterate over children and delete them
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			delete it->second;
		}
		m_elements.clear();
	}

	void Unit::logMe()
	{
		printf("--------\nName: %s\nPath: %s\nType: %s\n", m_pathname.c_str(), m_path.c_str(), m_type->getName().c_str());
	}
}