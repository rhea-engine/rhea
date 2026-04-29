/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Animation/Animator.h>

namespace rhea
{

	void Animator::PlayAnimation(Animation* animation)
	{
		m_current_animation = animation;
		m_current_time = 0.0f;
	}

	void Animator::onFrame(float delta)	
	{
		if (m_current_animation && m_skeleton) {
			m_current_time += delta;
			if (m_current_time > m_current_animation->duration) {
				m_current_time = fmod(m_current_time, m_current_animation->duration);
			}
			m_current_animation->UpdateSkeleton(m_skeleton, m_current_time);			
		}
	}
}
