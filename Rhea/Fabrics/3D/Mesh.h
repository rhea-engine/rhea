#ifndef RHEA_MESH_H
#define RHEA_MESH_H

#include <vector>

#include <Rhea/Unit.h>
#include <Rhea/Resources/VertexBuffer.h>

namespace Rhea
{
	class Application;
	class ImageResource;
	class Material;

	class Mesh : public Unit
	{
		MECLASS(Mesh, Unit)
	public:
		Mesh(std::string name, std::string type_name) :
			m_material(nullptr),
			m_isDynamic(false),
			Unit(name, type_name)
		{}
		Mesh(std::string name) :
			Mesh(name, "//meworla/mesh")
		{}

		struct vertex {
			float x, y, z, u, v;

			vertex() :
				x(0), y(0), z(0), u(0), v(0)
			{}
			vertex(float _x, float _y, float _z, float _u, float _v) :
				x(_x),
				y(_y),
				z(_z),
				u(_u),
				v(_v)
			{}

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

		// set buffer with map of attributes
		void setBuffers(std::unordered_map<std::string, VertexBuffer>& vertices, std::vector<unsigned long>& indices, size_t num_vertices);

		// return pointers to the buffers
		VertexBuffer* getVertexBuffer() { return &m_vertex; }
		const std::vector<unsigned long>& getIndices() const {
			return m_v_indices;
		}

	protected:
		Material* m_material;
		std::vector<vertex> m_vertices;
		std::vector<unsigned long> m_v_indices;
		std::vector<unsigned long> m_v_indices2;
		bool m_isDynamic;
		VertexBuffer m_vertex;
	};
}

#endif // !RHEA_MESH_H
