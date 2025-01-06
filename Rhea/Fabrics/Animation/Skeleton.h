#ifndef RHEA_SKELETON_H
#define RHEA_SKELETON_H

#include <Rhea/Unit.h>
#include "Bone.h"

namespace Rhea
{
	class Skeleton : public Unit
	{
		MECLASS(Skeleton, Unit)
	public:
		Skeleton(std::string name, std::string type_name) :			
			Unit(name, type_name)
		{}

		void UpdateBoneTransforms();

		void addBone(Bone bone) { m_bones.push_back(bone); }

		bool isValid();
		
		std::vector<Bone>& getBones() {	return m_bones;	}

		const std::vector<Math::Matrix4>& getBoneMatrices() const;

	protected:
		std::vector<Bone> m_bones;		
	};
}

#endif // !RHEA_SKELETON_H
