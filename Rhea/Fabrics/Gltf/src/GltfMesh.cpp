/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Gltf/GltfMesh.h>
#include <Psio/Log.h>
#include <Rhea/Gltf/GltfScene.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Item.h>

namespace rhea
{
	void GltfMesh::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "path", &GltfMesh::m_gltf_ref, "//meworla/geometryloader", ResourceUsage::Load);
	}

	bool GltfMesh::load(rhea::Mesh* mesh)
	{
		if (!m_gltf_ref.empty()) {
			GltfScene* scene = m_app->getUnitTree()->resolve(m_gltf_ref);
			if (scene) {
				return scene->load(mesh);
			}
			else {
				LOG(LOG_ERROR) << "GltfMesh: scene not found";				
			}
		}
		else {
			LOG(LOG_ERROR) << "GltfMesh: no path set";
		}

		return false;
	}
}