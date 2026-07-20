/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include <Rhea/Animation/AnimationState.h>
#include <Rhea/Animation/Animator.h>

namespace rhea
{
	void AnimationState::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values)
	{
		Component::setup(app, attributes, values);
		if (values.find("animation") != values.end())
		{
			m_animation_name = values["animation"].toString();
		}
		if (values.find("loop") != values.end())
		{
			loop = values["loop"].toBool();
		}
		if (values.find("speed") != values.end())
		{
			speed = values["speed"].toFloat();
		}
		if (values.find("weight") != values.end())
		{
			weight = values["weight"].toFloat();
		}
	}

	Animation* AnimationState::getAnimation()
	{
		if (!animation && !m_animation_name.empty())
		{
			// find the animation in the parent animator
			if (m_parent)
			{
				animation = doCast<Animator>(m_parent)->resolveAnimation(m_animation_name);
			}
		}
		return animation;
	}
}