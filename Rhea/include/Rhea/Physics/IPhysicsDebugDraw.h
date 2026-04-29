/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICSDEBUGDRAW_H
#define RHEA_PHYSICSDEBUGDRAW_H

namespace rhea 
{
    class IPhysicsDebugDraw
    {
    public:
        virtual ~IPhysicsDebugDraw() = default;
        
        virtual void setEnabled(bool on) = 0;      // toggle
        virtual bool isEnabled() const = 0;

        virtual void toggle() = 0;
    };

} // namespace rhea

#endif