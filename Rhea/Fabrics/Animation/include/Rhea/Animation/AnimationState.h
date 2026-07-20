/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_ANIMATIONSTATE_H
#define RHEA_ANIMATIONSTATE_H

#include <Rhea/Component.h>

namespace rhea
{
	class Animation;

	class AnimationState : public Component
	{
		RECLASS(AnimationState, Component, "//meworla/animationstate");
	public:
		AnimationState(std::string name, std::string type_name) :
			Component(name, type_name)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values) override;

		Animation* getAnimation();
		void setAnimation(Animation* anim) { animation = anim; }

		float speed = 1.0f;
		bool loop = true;
		std::string next_state;
		float weight = 1.0f;
	private:
		Animation* animation = nullptr;
		std::string m_animation_name;
	};
}

#endif // !RHEA_ANIMATIONSTATE_H