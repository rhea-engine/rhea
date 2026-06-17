/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_REF_H
#define RHEA_REF_H

#include <Rhea/UnitTree.h>
#include <SimpleParser/Type.h>

namespace rhea
{
	template <typename T>
	class Ref
	{
	public:
		Ref() = default;
		explicit Ref(std::string id) : m_id(std::move(id)) {}
		explicit Ref(std::string id, std::string base_path) : m_id(std::move(id)), m_base_path(std::move(base_path)) {}

		bool empty() const { return m_id.empty(); }
		explicit operator bool() const { return !empty(); }

		const std::string& id() const { return m_id; }
		void setId(std::string id) { m_id = std::move(id); }

		const std::string& basePath() const { return m_base_path; }
		void setBasePath(std::string base_path) { m_base_path = std::move(base_path); }

		T* resolve(UnitTree* tree) const { 
			if (m_base_path.empty())
				return doCast<T>(tree->find(m_id)); 
			else
				return doCast<T>(tree->find(m_id, m_base_path, false));
		}

		std::string_view typeName() const {
			if (auto type = simpleparser::Type::get(m_id))
				return type->getName();
			return {};
		}
	private:
		std::string m_id;
		std::string m_base_path;
	};
}

#endif // !RHEA_REF_H