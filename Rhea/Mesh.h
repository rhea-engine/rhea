#ifndef RHEA_MESH_H
#define RHEA_MESH_H

#include "Resource.h"

namespace Poseidon
{
	class Mesh;
	class Core;
}

namespace Rhea
{
	class Application;

	class Mesh : public Resource
	{
	public:
		Mesh(std::string name) :
			m_app(NULL),
			m_mesh(NULL),
			Resource(name, "//meworla/mesh")
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, std::string> values);

		// load mesh
		void load(ReadyHandler handler);

		Poseidon::Mesh* getMesh();
		// create the mesh from the resource strings
		void createMesh(std::string objbuf, std::string mtlbuf);

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		Application* m_app;
		Poseidon::Mesh* m_mesh;
	};
}

#endif // !RHEA_MESH_H
