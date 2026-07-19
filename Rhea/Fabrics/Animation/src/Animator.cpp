/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Animation/Animator.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>

#include <cmath>

namespace rhea
{
	void Animator::setup(Application* app, std::unordered_map<std::string, std::string> attributes,
		std::unordered_map<std::string, Variant> values)
	{
		Item::setup(app, std::move(attributes), std::move(values));

		const std::string base_path = m_attributes["basepath"];
		m_mesh_ref = Ref<Mesh>(getValueString("mesh"), base_path);
		m_skeleton_ref = Ref<Skeleton>(getValueString("skeleton"), base_path); // allow override of skeleton reference
		m_animation_ref = Ref<Animation>(getValueString("animation"), base_path);
		m_autoplay = getValue("autoplay", false);
	}

	void Animator::ready()
	{
		Item::ready();

		if (m_mesh_ref) {
			m_mesh = m_mesh_ref.resolve(m_app->getUnitTree());
			if (!m_mesh) {
				LOG(LOG_ERROR) << "Animator " << m_pathname << ": mesh " << m_mesh_ref.id() << " not found";
			}
			else {
				// Try to find a skeleton component in the mesh
				m_skeleton = m_mesh->getComponent<Skeleton>();
				if (!m_skeleton) {
					LOG(LOG_ERROR) << "Animator " << m_pathname << ": mesh " << m_mesh_ref.id() << " does not have a skeleton component";
				}
			}
		}

		if (m_skeleton_ref) {
			m_skeleton = m_skeleton_ref.resolve(m_app->getUnitTree());
			if (!m_skeleton) {
				LOG(LOG_ERROR) << "Animator " << m_pathname << ": skeleton " << m_skeleton_ref.id() << " not found";
			}
		}

		if (m_animation_ref) {
			m_current_animation = m_animation_ref.resolve(m_app->getUnitTree());
			if (!m_current_animation) {
				LOG(LOG_ERROR) << "Animator " << m_pathname << ": animation " << m_animation_ref.id() << " not found";
			}
		}
	}

	void Animator::onActivate()
	{
		m_active = true;
		if (m_autoplay && m_current_animation)
			PlayAnimation(m_current_animation);
	}

	void Animator::onDeactivate()
	{
		m_active = false;
	}

	void Animator::PlayAnimation(Animation* animation)
	{
		m_current_animation = animation;
		m_current_time = 0.0f;
	}

	void Animator::onFrame(float delta)	
	{
		if (m_active && m_current_animation && m_skeleton) {
			m_current_time += delta;
			if (m_current_animation->duration > 0.0f && m_current_time > m_current_animation->duration) {
				m_current_time = fmod(m_current_time, m_current_animation->duration);
			}
			m_current_animation->UpdateSkeleton(m_skeleton, m_current_time);			
		}
	}
}
