#ifndef RHEA_PHYSICSDEBUGDRAW_H
#define RHEA_PHYSICSDEBUGDRAW_H

namespace Rhea 
{
    class IPhysicsDebugDraw
    {
    public:
        virtual ~IPhysicsDebugDraw() = default;
        
        virtual void setEnabled(bool on) = 0;      // toggle
        virtual bool isEnabled() const = 0;

        virtual void toggle() = 0;
    };

} // namespace Rhea

#endif