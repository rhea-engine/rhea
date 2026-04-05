#ifndef POSEIDON_ANIMATABLE_H
#define POSEIDON_ANIMATABLE_H

#include "Renderable.h"

namespace poseidon
{
	class Viewport;
	class Camera;

	/** An object that can be animated ie can be updated per frame or per timeslot and can be attached to a node
	*/
	class Animatable : public Renderable
	{
	public:
		virtual bool onUpdate() = 0;
	};
}
#endif // !POSEIDON_ANIMATABLE_H
