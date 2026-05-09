/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICSDEBUGDRAW_H
#define RHEA_PHYSICSDEBUGDRAW_H

#include <Rhea/Core/Item.h>

namespace rhea 
{
    class IPhysicsDebugDraw : public Item
    {
        RECLASS_VIRTUAL(IPhysicsDebugDraw, Item, "//meworla/if_physicsdebugdraw")
    public:
        IPhysicsDebugDraw(std::string name, std::string type_name) :
            Item(name, type_name)
		{
		}

        virtual ~IPhysicsDebugDraw() = default;
        
        virtual void setEnabled(bool on) = 0;      // toggle
        virtual bool isEnabled() const = 0;

        virtual void toggle() = 0;
    };

} // namespace rhea

#endif