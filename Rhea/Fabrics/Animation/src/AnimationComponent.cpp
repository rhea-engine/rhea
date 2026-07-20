#include <Rhea/Animation/AnimationComponent.h>
#include <Rhea/Components/MeshComponent.h>
#include <Psio/Log.h>
#include <Rhea/Application.h>

namespace rhea
{

	void AnimationComponent::ready()
	{
		if (m_values.find("animatorRef") != m_values.end())
			m_animator_ref = Ref<Animator>(m_values["animatorRef"].toString(), m_attributes["basepath"]);
		if (m_parent && !m_parent->getComponent<AnimationComponent>()) {
			m_parent->addComponent(this);
		}

		if (m_animator_ref.id().empty()) {
			LOG(LOG_ERROR) << "AnimationComponent: animatorRef is empty";
			return;
		}
		m_animator = m_animator_ref.resolve(m_app->getUnitTree());
		if (!m_animator) {
			LOG(LOG_ERROR) << "AnimationComponent: animator " << m_animator_ref.id() << " not found";
			return;
		}

		// find skeleton from parent
		if (m_parent) {
			auto* mc = m_parent->getComponent<MeshComponent>();
			if (!mc) {
				LOG(LOG_ERROR) << "Animation for " << m_parent->getPath() << ": does not have a mesh component";
			}
			else {
				m_mesh = mc->getMesh();

				if (!m_mesh) {
					Ref<Mesh> m_mesh_ref = mc ? mc->meshRef() : Ref<Mesh>();
					m_mesh = m_mesh_ref.resolve(m_app->getUnitTree());
					if (!m_mesh) {
						LOG(LOG_ERROR) << "Animation for " << m_parent->getPath() << ": mesh " << m_mesh_ref.id() << " not found";
						return;
					}
				}
				// Try to find a skeleton component in the mesh
				m_skeleton = m_mesh->getComponent<Skeleton>();
				// perhaps not initialized yet, so we can try to resolve the skeleton from the mesh later in ensureSkeleton()
			}
		}		
	}

	bool AnimationComponent::ensureSkeleton()
	{
		if (m_skeleton)
			return true;
		m_skeleton = m_mesh->getComponent<Skeleton>();
		if (!m_skeleton) {
			LOG(LOG_ERROR) << "Animation for " << m_parent->getPath() << ": mesh does not have a skeleton component";
			return false;
		}
		return true;
	}

	void AnimationComponent::onActivate()
	{
		m_active = true;
		if (!m_animator || !ensureSkeleton()) {
			m_active = false;
			return;
		}
		if (!m_animation_context) {
			std::string animation_id = m_values.find("animation") != m_values.end() ? m_values["animation"].toString() : "";
			auto* context = m_animator->createAnimationContext(animation_id);
			if (context) {
				m_animation_context.reset(context);
				m_animation_context->component = this;
			}
			else
				LOG(LOG_ERROR) << "AnimationComponent: failed to create animation state for animator " << m_animator_ref.id();
		}

		auto& bones = m_skeleton->getBones();
		m_bone_poses.clear();
		m_bone_poses.resize(bones.size());
		if (m_animation_context) {
			m_animation_context->current_time = 0.0f;
			m_animation_context->is_playing = true;
		}
	}

	void AnimationComponent::onDeactivate()
	{
		m_active = false;
	}

	void AnimationComponent::onFrame(float delta)
	{
		if (m_active && m_animator && ensureSkeleton()) {
			m_animator->onFrame(delta, m_animation_context.get());
		}
	}

	const std::vector<math::Matrix4>& AnimationComponent::getFinalBoneMatrices() const
	{
		m_finalBoneMatrices.clear();
		m_finalBoneMatrices.reserve(m_bone_poses.size());

		for (const auto& bone_pose : m_bone_poses) {
			m_finalBoneMatrices.push_back(bone_pose.final_transform.transpose());
		}
		return m_finalBoneMatrices;
	}

	void AnimationComponent::playAnimation(Animation* animation)
	{
		if (animation && m_animator) {
			if (!m_animation_context) {
				auto* context = m_animator->createAnimationContext(animation->getPathName());
				if (context)
					m_animation_context.reset(context);
				else {
					LOG(LOG_ERROR) << "AnimationComponent: failed to create animation state for animator " << animation->getPathName();
					return;
				}
			}
			else 
				m_animation_context->animation = animation;
			m_animation_context->current_time = 0.0f;
			m_animation_context->is_playing = true;
		}
		else {
			LOG(LOG_ERROR) << "AnimationComponent: invalid animation or animator";
		}
	}

	Animation* AnimationComponent::getCurrentAnimation() const
	{
		if (m_animation_context) {
			return m_animation_context->animation;
		}
		return nullptr;
	}

	BonePose* AnimationComponent::getBonePose(int index) {
		if (index < 0 || index >= m_bone_poses.size())
			return nullptr;
		return &m_bone_poses[index];
	}

	void AnimationComponent::UpdateBoneTransforms()
	{
		if (!ensureSkeleton())
			return;

		auto& bones = m_skeleton->getBones();
		for (int i = 0; i < bones.size(); ++i) {
			if (bones[i].parent_index == -1) {
				// Root bone, no parent, so global transform is just the local transform
				m_bone_poses[i].UpdateGlobalTransform(math::Matrix4(), bones[i].inverse_bind_matrix); // Identity matrix
			}
			else {
				// Update based on parent's global transform							
				m_bone_poses[i].UpdateGlobalTransform(m_bone_poses[bones[i].parent_index].global_transform, bones[i].inverse_bind_matrix);
			}
		}
	}
}
