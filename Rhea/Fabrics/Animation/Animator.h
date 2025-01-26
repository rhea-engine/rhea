#ifndef RHEA_ANIMATOR_H
#define RHEA_ANIMATOR_H

#include <Rhea/Unit.h>
#include <Rhea/Fabrics/Core/Item.h>
#include <Rhea/Fabrics/Animation/Skeleton.h>
#include "Animation.h"

namespace Rhea
{
	class Animator : public Item
	{
		MECLASS(Animator, Item)
	public:

		Animator(std::string name, std::string typename) :
			m_skeleton(nullptr),
			Item(name, "//meworla/animator")
		{}

        void PlayAnimation(Animation* animation);
       
		void onFrame(float delta);
			
		void setSkeleton(Skeleton* skeleton) { m_skeleton = skeleton; }
        
    private:
        Animation* m_current_animation = nullptr;
        Skeleton* m_skeleton;
        float m_current_time = 0.0f;
	};
}

#endif // !RHEA_ANIMATOR_H
