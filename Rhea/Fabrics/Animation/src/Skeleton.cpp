#include <Rhea/Animation/Skeleton.h>
#include <Psio/Log.h>
#include <cassert>

namespace rhea
{
	void Skeleton::UpdateBoneTransforms()
	{		
		for (int i = 0; i < m_bones.size(); ++i) {
			if (m_bones[i].parent_index == -1) {
				// Root bone, no parent, so global transform is just the local transform
				m_bones[i].UpdateGlobalTransform(math::Matrix4()); // Identity matrix
			}
			else {
				// Update based on parent's global transform							
				m_bones[i].UpdateGlobalTransform(m_bones[m_bones[i].parent_index].global_transform);
			}
		}
	}

	bool Skeleton::isValid()
	{
		// check if sorted
		for (int i = 0; i < m_bones.size(); ++i) {
			// Update based on parent's global transform
			if (m_bones[i].parent_index != -1 && m_bones[i].parent_index >= m_bones.size()) {
				LOG(LOG_ERROR) << "Error: Invalid parent index for bone " << i;	
				assert(0);
				return false;
			}
		}
		UpdateBoneTransforms();
		for (int i = 0; i < m_bones.size(); ++i) {
			math::Matrix4 testIdentity = m_bones[i].inverse_bind_matrix * m_bones[i].global_transform;
			
			if (!testIdentity.isApproximatelyIdentity()) {
				DLOG(LOG_DEBUG) << "Bone " << i << " (" << m_bones[i].name << ") Identity Check:";// << testIdentity;
				// Check if testIdentity is approximately the identity matrix			
				LOG(LOG_ERROR) << "Error: Inverse bind matrix for bone " << m_bones[i].name << " is incorrect.";
				//return false;
			}
#ifdef DEBUG_BONES
			else
				DLOG(LOG_DEBUG) << "Bone " << i << " (" << m_bones[i].name << ") Identity Check: ok";// << testIdentity;
#endif
		}

		// debug output
#ifdef DEBUG_BONES
		for (int i = 0; i < 3; i++) {
			LOG(LOG_DEBUG) << "Inverse bind matrix for bone " << i;
			for (int r = 0; r < 4; r++)
				LOG(LOG_DEBUG) << "[ " << m_bones[i].inverse_bind_matrix[r][0] << ", " << m_bones[i].inverse_bind_matrix[r][1] << ", " << m_bones[i].inverse_bind_matrix[r][2] << ", " << m_bones[i].inverse_bind_matrix[r][3]<< " ]";
			LOG(LOG_DEBUG) << "local transform matrix for bone " << i;
			for (int r = 0; r < 4; r++)
				LOG(LOG_DEBUG) << "[ " << m_bones[i].local_transform[r][0] << ", " << m_bones[i].local_transform[r][1] << ", " << m_bones[i].local_transform[r][2] << ", " << m_bones[i].local_transform[r][3] << " ]";
			LOG(LOG_DEBUG) << "global transform matrix for bone " << i;
			for (int r = 0; r < 4; r++)
				LOG(LOG_DEBUG) << "[ " << m_bones[i].global_transform[r][0] << ", " << m_bones[i].global_transform[r][1] << ", " << m_bones[i].global_transform[r][2] << ", " << m_bones[i].global_transform[r][3] << " ]";

		}
#endif
		
		return true;
	}	

	const std::vector<math::Matrix4>& Skeleton::getBoneMatrices() const
	{
		static std::vector<math::Matrix4> boneMatrices;
		boneMatrices.clear();
		for (const auto& bone : m_bones) {
			boneMatrices.push_back(bone.final_transform.transpose());
		}
		return boneMatrices;
	}
}