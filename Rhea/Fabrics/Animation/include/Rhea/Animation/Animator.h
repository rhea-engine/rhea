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
#include <Rhea/Core/Mesh.h>
#include <Rhea/Ref.h>
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
			m_mesh(nullptr),
			Item(name, "//meworla/animator")
		{}
		void setup(Application* app) override { Item::setup(app, {}, {}); }
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;
		void ready() override;
		void onActivate() override;
		void onDeactivate() override;

        void PlayAnimation(Animation* animation);
       
		void onFrame(float delta) override;
			
		void setSkeleton(Skeleton* skeleton) { m_skeleton = skeleton; }

		Animation* getCurrentAnimation() const { return m_current_animation; }
        
    private:
		Ref<Mesh> m_mesh_ref;
		Ref<Skeleton> m_skeleton_ref;
		Ref<Animation> m_animation_ref;
        Animation* m_current_animation = nullptr;
		Mesh* m_mesh = nullptr;
        Skeleton* m_skeleton = nullptr;
        float m_current_time = 0.0f;
		bool m_autoplay = false;
		bool m_active = false;
	};
}

#endif // !RHEA_ANIMATOR_H
