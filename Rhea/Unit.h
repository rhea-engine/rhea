#ifndef RHEA_UNIT_H
#define RHEA_UNIT_H

#include <string>
#include <vector>
#include <unordered_map>
#include "SimpleParser/Variant.h"

using SimpleParser::Variant;

namespace Rhea
{
	class Application;

	class Unit
	{
	public:
		Unit(std::string path_name, std::string type_name) :
			m_app(NULL),
			m_pathname(path_name),
			m_type_name(type_name),
			m_parent(NULL),
			m_path(path_name)
		{}		
		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		virtual void setup(Application* app) { m_app = app; }

		// add a child resource
		void addChild(Unit* child);

		// get name
		std::string getPathName() { return m_pathname; }

		// get full path
		std::string getPath() { return m_path; }

		// get my type
		std::string getType() { return m_type_name; }

		// find by path
		Unit* find(std::string path, bool create = false);

		// find nearest by type
		Unit* findByType(std::string type_name);

		// messaging
		void broadcast(std::string method_name);
		virtual void message(std::string method_name) { if (method_name == "logme") logMe(); }

		// print my info
		virtual void logMe();
	protected:
		Application* m_app;
		Unit* m_parent;
		std::string m_pathname;
		std::string m_path;
		std::string m_type_name;
		std::unordered_map<std::string, std::string> m_attributes;
		std::unordered_map<std::string, Variant> m_values;
		std::unordered_map<std::string, Unit*> m_children;
	};
}

#endif // !RHEA_UNIT_H
