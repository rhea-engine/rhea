#ifndef RHEA_MESH_H
#define RHEA_MESH_H

#include <vector>

#include <Rhea/Unit.h>

namespace Rhea
{
	class Application;
	class ImageResource;
	class Material;

	class Mesh : public Unit
	{
	public:
		Mesh(std::string name) :
			m_material(nullptr),
			m_isDynamic(false),
			Unit(name, "//meworla/mesh")
		{}

		struct vertex {
			float x, y, z, u, v;

			bool operator==(const vertex& other) const
			{
				return (x == other.x
					&& y == other.y
					&& z == other.z
					&& u == other.u
					&& v == other.v
					);
			}
		};

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		std::vector<Unit::Export> getReferences();

		// ready handler
		void ready();
				
		// get material
		Material* getMaterial() { return m_material; }
		void setMaterial(Material* material) { m_material = material; }

		// do we need dynamic buffers?
		void setDynamic(bool is_dynamic) { m_isDynamic = is_dynamic; }
		bool isDynamic() { return m_isDynamic; }

		// return the buffers
		void getBuffers(std::vector<vertex>& vertices, std::vector<unsigned long>& indices);
		void setBuffers(std::vector<vertex>& vertices, std::vector<unsigned long>& indices);

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		Material* m_material;
		std::vector<vertex> m_vertices;
		std::vector<unsigned long> m_v_indices;
		bool m_isDynamic;
	};
}

#endif // !RHEA_MESH_H
