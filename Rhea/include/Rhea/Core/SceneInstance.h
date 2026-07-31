/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SCENEINSTANCE_H
#define RHEA_SCENEINSTANCE_H

#include <Rhea/Core/Item.h>

namespace rhea
{
    class SceneInstance : public rhea::Item
    {
        RECLASS(SceneInstance, rhea::Item, "//meworla/sceneinstance")

    public:
        SceneInstance(std::string name, std::string type)
            : Item(name, type)
        {}

        std::vector<ResourceReference> getReferences() override
        {
			std::vector<ResourceReference> refs = Item::getReferences();
            refs.push_back({
                "source",
                "//meworla/scene",
                getValue("source"),
                ResourceUsage::Instantiate
                });
			return refs;
        }
    };
}

#endif // !RHEA_SCENEINSTANCE_H