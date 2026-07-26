/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_SCENEENTRYPOINT_H
#define RHEA_SCENEENTRYPOINT_H

#include <Rhea/Core/Item.h>

namespace rhea
{
    class SceneEntryPoint : public Item
    {
        RECLASS(SceneEntryPoint, Item, "//meworla/entrypoint")

    public:
		SceneEntryPoint(std::string name, std::string type_name) :
			Item(name, type_name)
		{}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override
		{
			Item::setup(app, attributes, values);
            
			m_entry_id = getValueString("entry_id", getPathName());
		}

		void setEntryId(const std::string& entry_id)
		{
			m_entry_id = entry_id;
		}

        std::string getEntryId() const
        {
            return m_entry_id;
        }

    protected:
		std::string m_entry_id;
    };
}

#endif // !RHEA_SCENEENTRYPOINT_H
