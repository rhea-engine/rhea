/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_ANIMATIONCOMPONENT_H
#define RHEA_ANIMATIONCOMPONENT_H

#include <Rhea/Animation/Animator.h>
#include <Rhea/Ref.h>
#include <string>
#include <memory>
#include <Rhea/Components/ActiveComponent.h>

namespace rhea
{
    class Mesh;
    class Skeleton;

    /** Manages animation of skeletal mesh */
    class AnimationComponent : public ActiveComponent
    {
        RECLASS(AnimationComponent, ActiveComponent, "//meworla/animationcomponent");
        REPROPERTY(Ref<Animator>, "animator_ref", Label("Animator Reference"), Edit, Read);
    public:
        AnimationComponent(std::string name, std::string type_name) :
            ActiveComponent(name, type_name),
            m_animator(nullptr)
        {}
        void ready() override;

        // Asset reference (unresolved)
        const Ref<Animator>& animatorRef() const { return m_animator_ref; }
        void setAnimatorRef(Ref<Animator> ref) {
            m_animator_ref = std::move(ref);
        }

        // play api
		void play(std::string state_name);
        void pause();
		void resume();
        bool isPlaying() const;
		std::string getCurrentStateName() const;

        // Resolved asset
        Animator* getAnimator() const { return m_animator; }
        void setAnimator(Animator* anim) { m_animator = anim; }

        void onActivate() override;
        void onDeactivate() override;

        void onFrame(float delta) override;

        BonePose* getBonePose(int index);
        void UpdateBoneTransforms();
        const std::vector<math::Matrix4>& getFinalBoneMatrices() const;

		// playing a specific animation without state management
		void playAnimation(Animation* animation);
		Animation* getCurrentAnimation() const;

    private:
        // Unresolved reference
        Ref<Animator> m_animator_ref;

        // Resolved instance
        Animator* m_animator = nullptr;
		// Resolved skeleton
		Skeleton* m_skeleton = nullptr;
		// Resolved mesh
		Mesh* m_mesh = nullptr;

		bool m_active = false;

        std::vector<BonePose> m_bone_poses;
        mutable std::vector<math::Matrix4> m_finalBoneMatrices;

		std::unique_ptr<Animator::AnimationContext> m_animation_context;

		bool ensureSkeleton();
    };
}
#endif