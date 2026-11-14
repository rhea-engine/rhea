#include <Rhea/Gltf/GltfGeometry.h>
#include <Rhea/Gltf/GltfScene.h>
#include <Psio/Log.h>

namespace rhea
{
	GltfGeometry::GltfGeometry(std::string path_name, std::string type_name) :
		Geometry(path_name, type_name)
	{}
	void GltfGeometry::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Geometry::setup(app, attributes, values);
		// register as component if not already registered
		if (m_parent && !m_parent->getComponent<GltfGeometry>()) {
			m_parent->addComponent(this);
		}
	}

	bool GltfGeometry::load(Mesh* mesh)
	{
		GltfScene* loader = doCast<GltfScene>(getParent());
		if (!loader) {
			LOG(LOG_ERROR) << "GltfGeometry::load: parent is not a GltfScene";
			return false;
		}

		// load the gltf file and populate the mesh
		return loader->load(mesh);
	}
}