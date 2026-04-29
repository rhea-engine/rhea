/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_ANIMATOR_H
#define RHEA_ANIMATOR_H

#include <Rhea/Unit.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Animation/Skeleton.h>
#include "Animation.h"

namespace rhea
{
	class Animator : public Item
	{
		MECLASS(Animator, Item)
	public:

		Animator(std::string name, std::string type_name) :
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
