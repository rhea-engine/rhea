#include "Skeleton.h"

namespace Rhea
{
	void Skeleton::UpdateBoneTransforms()
	{
		for (int i = 0; i < m_bones.size(); ++i) {
			if (m_bones[i].parent_index == -1) {
				// Root bone, no parent, so global transform is just the local transform
				m_bones[i].UpdateGlobalTransform(Math::Matrix4()); // Identity matrix
			}
			else {
				// Update based on parent's global transform
				m_bones[i].UpdateGlobalTransform(m_bones[m_bones[i].parent_index].global_transform);
			}
		}
	}
	const std::vector<Math::Matrix4>& Skeleton::getBoneMatrices() const
	{
		static std::vector<Math::Matrix4> boneMatrices;
		boneMatrices.clear();
		for (const auto& bone : m_bones) {
			boneMatrices.push_back(bone.final_transform);
		}
		return boneMatrices;
	}
}