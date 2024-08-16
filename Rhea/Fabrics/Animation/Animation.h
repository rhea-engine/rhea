#ifndef RHEA_ANIMATION_H
#define RHEA_ANIMATION_H

#include <Rhea/Unit.h>

namespace Rhea
{
	class Animation : public Unit
	{
		MECLASS(Animation, Unit)
	public:
		
		Animation(std::string name, std::string typename) :
			Unit(name, "//meworla/scene")
		{}
	};
}
#endif // !RHEA_ANIMATION_H
