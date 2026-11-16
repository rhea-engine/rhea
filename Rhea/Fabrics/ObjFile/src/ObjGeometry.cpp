#include <Rhea/ObjFile/ObjGeometry.h>
#include <Rhea/ObjFile/ObjDocument.h>
#include <Psio/Log.h>

namespace rhea
{
	ObjGeometry::ObjGeometry(std::string path_name, std::string type_name) :
		Geometry(path_name, type_name)
	{}

	void ObjGeometry::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Geometry::setup(app, attributes, values);

		// register as component if not already registered
		if (m_parent && !m_parent->getComponent<ObjGeometry>()) {
			m_parent->addComponent(this);
		}
	}

	bool ObjGeometry::load(Mesh* mesh)
	{
		ObjDocument* loader = doCast<ObjDocument>(getParent());
		if (!loader) {
			LOG(LOG_ERROR) << "ObjGeometry::load: parent is not an ObjDocument";
			return false;
		}

		// load the obj file and populate the mesh
		return loader->load(mesh);
	}
}