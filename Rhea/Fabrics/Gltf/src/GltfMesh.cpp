#include <Rhea/Gltf/GltfMesh.h>
#include <Psio/Log.h>
#include <Rhea/Gltf/GltfScene.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Item.h>

namespace rhea
{
	std::vector<Unit::Export> GltfMesh::getReferences()
	{
		std::vector<Export> refs;
		if (m_values.find("path") != m_values.end())
			refs.push_back({ "path", "//meworla/geometryloader", m_values["path"] });
		return refs;
	}

	bool GltfMesh::load(rhea::Mesh* mesh)
	{
		if (m_values.find("path") != m_values.end()) {
			GltfScene* scene = doCast<GltfScene>(m_app->getUnitTree()->find(m_values["path"].toString(), m_attributes["basepath"], false));
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