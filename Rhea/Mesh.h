#ifndef RHEA_MESH_H
#define RHEA_MESH_H

#include <vector>

#include "Resource.h"

namespace Rhea
{
	class Application;
	class ImageResource;

	class Mesh : public Resource
	{
	public:
		Mesh(std::string name) :
			m_app(nullptr),		
			Resource(name, "//meworla/mesh")
		{}

		struct vertex {
			float x, y, z, u, v;
		};

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// load mesh
		void load(ReadyHandler handler);
				
		// return the buffers
		void getBuffers(std::vector<vertex>& vertices, std::vector<unsigned long>& indices);

		// get textures
		std::vector<ImageResource*>& getTextures() { return m_textures; }

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		Application* m_app;		
		std::vector<vertex> m_vertices;
		std::vector<unsigned long> m_v_indices;
		std::vector<ImageResource*> m_textures;
	};
}

#endif // !RHEA_MESH_H
