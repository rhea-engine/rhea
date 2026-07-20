/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SKELETON_H
#define RHEA_SKELETON_H

#include <Math/Matrix4.h>
#include <Rhea/Component.h>
#include "Bone.h"

namespace rhea
{
	class Skeleton : public Component
	{
		MECLASS(Skeleton, Component)
	public:
		Skeleton(std::string name, std::string type_name) :			
			Component(name, type_name)
		{}

		void addBone(Bone bone) { m_bones.push_back(bone); }

		bool isValid();
		
		std::vector<Bone>& getBones() {	return m_bones;	}

	protected:
		std::vector<Bone> m_bones;	
	};
}

#endif // !RHEA_SKELETON_H
