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
#include <Rhea/Animation/AnimationState.h>

namespace rhea
{
	void Animator::setup(Application* app, std::unordered_map<std::string, std::string> attributes,
		std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, std::move(attributes), std::move(values));

		const std::string base_path = m_attributes["basepath"];
		m_mesh_ref = Ref<Mesh>(getValueString("mesh"), base_path);
		m_skeleton_ref = Ref<Skeleton>(getValueString("skeleton"), base_path); // allow override of skeleton reference
		m_initial_state = getValueString("initialState");
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

	Animator::AnimationContext* Animator::createAnimationContext(const std::string& initial_state)
	{
		AnimationContext* context = nullptr;
		if (!ensureSkeleton()) {
			return nullptr;
		}
		AnimationState* animation_state = nullptr;
		if (m_mesh) {
			if (!initial_state.empty()) {
				animation_state = doCast<AnimationState>(getChild(initial_state));
				if (!animation_state) {
					LOG(LOG_ERROR) << "Animator " << m_pathname << ": animation state " << initial_state << " not found";
				}
			}
			else if (!m_initial_state.empty()) {
				// try default initial state
				animation_state = doCast<AnimationState>(getChild(m_initial_state));
				if (!animation_state) {
					LOG(LOG_ERROR) << "Animator " << m_pathname << ": animation state " << m_initial_state << " not found";
				}
			}
		}
		else {
			LOG(LOG_ERROR) << "Animator " << getPath() << ": no mesh specified";
		}
		context = new AnimationContext(animation_state);
		return context;
	}

	bool Animator::play(AnimationContext& context, const std::string& state_name)
	{
		auto state = doCast<AnimationState>(getChild(state_name));

		if (!state) {
			LOG(LOG_ERROR) << "Animator " << getPath() << ": unknown state " << state_name;
			return false;
		}

		context.current_state = state;
		context.current_time = 0.0f;
		context.playing = true;
		context.started = true;
		return true;
	}

	void Animator::onFrame(float delta, AnimationContext* context)
	{
		if (context && context->playing && context->current_state && m_skeleton) {
			context->current_time += delta * context->speed;
			Animation* animation = context->current_state->getAnimation();
			if (!animation) {
				return;
			}
			if (animation->duration > 0.0f && context->current_time > animation->duration) {
				context->current_time = fmod(context->current_time, animation->duration);
			}
			animation->UpdateSkeleton(context->component, m_skeleton, context->current_time);			
		}
	}

	Animation* Animator::resolveAnimation(const std::string& animation_id)
	{
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
		}
		else {
			LOG(LOG_ERROR) << "Animator " << getPath() << ": no mesh specified";
		}
		return animation;
	}
}
