/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_ANIMATOR_H
#define RHEA_ANIMATOR_H

#include <Rhea/Unit.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Ref.h>
#include <Rhea/Animation/Skeleton.h>
#include "Animation.h"

namespace rhea
{
	class AnimationComponent;

	class Animator : public Unit
	{
		MECLASS(Animator, Unit)
	public:
		struct AnimationContext {
			Animation* animation;
			AnimationComponent* component;
			float current_time;
			float speed;
			bool is_playing;
			AnimationContext(Animation* anim) : animation(anim), component(nullptr), current_time(0.0f), speed(1.0f), is_playing(false) {}
		};

		Animator(std::string name, std::string type_name) :
			m_skeleton(nullptr),
			m_mesh(nullptr),
			Unit(name, "//meworla/animator")
		{}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;
		void ready() override;

		// return a fresh animation context for the given animation id
		// if animation id is empty use the default animation id
		AnimationContext* createAnimationContext(const std::string& animation_id);
      
		void onFrame(float delta, AnimationContext* context);
			
		void setSkeleton(Skeleton* skeleton) { m_skeleton = skeleton; }

        
    private:
		Ref<Mesh> m_mesh_ref;
		Ref<Skeleton> m_skeleton_ref;
		std::string m_animation_id;
		Animation* m_default_animation = nullptr;
		Mesh* m_mesh = nullptr;
        Skeleton* m_skeleton = nullptr;

		bool m_autoplay = false;

		bool ensureSkeleton();
	};
}

#endif // !RHEA_ANIMATOR_H
