#ifndef POSEIDON_SKELETONPOSE_H
#define POSEIDON_SKELETONPOSE_H

#include <Math/Matrix4.h>

namespace poseidon
{
    using SkeletonPoseHandle = uint32_t;
    constexpr SkeletonPoseHandle InvalidSkeletonPoseHandle = 0;

    class SkeletonPose
    {
    public:
        void setBoneCount(size_t count)
        {
            m_bone_matrices.resize(count);
        }

        size_t getBoneCount() const
        {
            return m_bone_matrices.size();
        }

        void setBoneMatrix(size_t index, const math::Matrix4& mat)
        {
            m_bone_matrices[index] = mat;
        }

        const std::vector<math::Matrix4>& getBoneMatrices() const
        {
            return m_bone_matrices;
        }

        std::vector<math::Matrix4>& accessBoneMatrices()
        {
            return m_bone_matrices;
        }

    private:
        std::vector<math::Matrix4> m_bone_matrices;
    };
}

#endif // !POSEIDON_SKELETONPOSE_H
