/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_UnitTREE_H
#define RHEA_UnitTREE_H

#include <string>
#include <unordered_map>
#include <functional>

#include "Unit.h"
#include "Ref.h"

namespace rhea
{
	class Application;

	// the class that holds a Unit tree 
	// it is also a Unit factory
	class UnitTree : public Unit
	{
		MECLASS(UnitTree, Unit)
	public:

		class Binding {
		public:
			Unit* target = nullptr;
			std::string value;
		};

		UnitTree(std::string basename, std::string type_name) :
			Unit(basename, type_name)
		{}

		// create a reource
		Unit* createUnit(std::string basename, std::string type_name, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		
		template<typename T>
		T* createUnit(std::string basename, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) {
			T* ret = simpleparser::Type::create<T>(basename);
			if (doCast<Unit>(ret))
				ret->setup(m_app, attributes, values);
			return ret;
		}

		// create resource without values
		Unit* createUnit(std::string basename, std::string type_name);

		// create a resource with res id and no values
		Unit* createUnitById(std::string resource_id, std::string type_name);

		// get a binding via path
		Binding getBinding(std::string binding_desc, std::string base);

		template<class T>
		T* resolve(const Ref<T>& reference)
		{
			if (reference.empty())
				return nullptr;

			if (reference.basePath().empty())
				return doCast<T>(find(reference.id()));

			return doCast<T>(
				find(reference.id(), reference.basePath(), false));
		}
	};
}

#endif // !RHEA_UnitTREE_H
