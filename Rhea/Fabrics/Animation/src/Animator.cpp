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
#include <Rhea/Components/MeshComponent.h>
#include <cmath>

namespace rhea
{
	void Animator::setup(Application* app, std::unordered_map<std::string, std::string> attributes,
		std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, std::move(attributes), std::move(values));

		const std::string base_path = m_attributes["basepath"];
		m_mesh_ref = Ref<Mesh>(getValueString("mesh"), base_path);
		m_skeleton_ref = Ref<Skeleton>(getValueString("skeleton"), base_path); // allow override of skeleton reference
		m_animation_id = getValueString("animation");
		m_autoplay = getValue("autoplay", false);
	}

	void Animator::ready()
	{
		Unit::ready();
	}

	bool Animator::ensureSkeleton()
	{
		if (m_skeleton)
			return true;

		if (!m_mesh && m_mesh_ref) {
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
				else
					return true;
			}
		}

		if (m_skeleton_ref) {
			m_skeleton = m_skeleton_ref.resolve(m_app->getUnitTree());
			if (!m_skeleton) {
				LOG(LOG_ERROR) << "Animator " << m_pathname << ": skeleton " << m_skeleton_ref.id() << " not found";
				return false;
			}
			else {
				// also try to find mesh from skeleton if not already set
				if (!m_mesh && doCast<Mesh>(m_skeleton->getParent())) {
					m_mesh = doCast<Mesh>(m_skeleton->getParent());
				}
				return true;
			}
		}
		return false;
	}

	Animator::AnimationContext* Animator::createAnimationContext(const std::string& animation_id)
	{
		AnimationContext* context = nullptr;
		if (!ensureSkeleton()) {
			return nullptr;
		}
		Animation* animation = nullptr;
		if (m_mesh) {
			if (!animation_id.empty()) {
				animation = doCast<Animation>(m_mesh->getChild(animation_id));
				if (!animation) {
					LOG(LOG_ERROR) << "Animator " << m_pathname << ": animation " << animation_id << " not found";
				}
			}
			else if (!m_animation_id.empty()) {
				// try default animation id
				animation = doCast<Animation>(m_mesh->getChild(m_animation_id));
				if (!animation) {
					LOG(LOG_ERROR) << "Animator " << m_pathname << ": animation " << m_animation_id << " not found";
				}
			}
		}
		else {
			LOG(LOG_ERROR) << "Animator " << getPath() << ": no mesh specified";
		}
		context = new AnimationContext(animation);
		return context;
	}

	void Animator::onFrame(float delta, AnimationContext* context)
	{
		if (context && context->is_playing && context->animation && m_skeleton) {
			context->current_time += delta * context->speed;
			if (context->animation->duration > 0.0f && context->current_time > context->animation->duration) {
				context->current_time = fmod(context->current_time, context->animation->duration);
			}
			context->animation->UpdateSkeleton(context->component, m_skeleton, context->current_time);			
		}
	}
}
