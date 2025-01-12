#ifndef RHEA_MESH_H
#define RHEA_MESH_H

#include <vector>

#include <Rhea/Unit.h>
#include <Rhea/Resources/VertexBuffer.h>
#include <Rhea/Resources/VertexAttribute.h>
#include <Rhea/Fabrics/Animation/Skeleton.h>

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
			m_state(0),
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

		Mesh* Clone() const override {
			return new Mesh(*this); // Use the copy constructor
		}

		struct SubMesh {
			std::vector<unsigned long> indices;
			Material* material;
		};
		
		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		std::vector<Unit::Export> getReferences();

		// ready handler
		void ready();
				
		// get material
		Material* getMaterial(size_t mesh_idx = 0) { if (mesh_idx < m_submeshes.size()) return m_submeshes[mesh_idx].material; else return nullptr; }
		void setMaterial(Material* material);

		// do we need dynamic buffers?
		void setDynamic(bool is_dynamic) { m_isDynamic = is_dynamic; }
		bool isDynamic() { return m_isDynamic; }	

		// return the buffers - short hand for meshes without submeshes
		void getBuffers(std::vector<vertex>& vertices, std::vector<unsigned long>& indices);
		void setBuffers(std::vector<vertex>& vertices, std::vector<unsigned long>& indices);

		// set buffer with map of attributes
		void setVertexBuffer(std::vector<VertexAttribute>& layout, std::unordered_map<std::string, VertexBuffer>& vertices, size_t num_vertices);
		// short hand for mehses with no submeshes
		void setBuffers(std::unordered_map<std::string, VertexBuffer>& vertices, std::vector<unsigned long>& indices, size_t num_vertices);

		// set submeshes with material and indices
		void addSubmesh(Material* material, std::vector<unsigned long>& indices);
		void setSubmesh(size_t idx, Material* material, std::vector<unsigned long>& indices);

		// get number of submeshes
		size_t getNumSubmeshes() { return m_submeshes.size(); }

		// get default Material
		Material* getDefaultMaterial() { return m_material; }

		// return pointers to the buffers
		VertexBuffer* getVertexBuffer() { return &m_vertex; }
		VertexBuffer& getVertexBufferRef() { return m_vertex; }
		const std::vector<unsigned long>& getIndices(size_t mesh_idx) const {			
			if (mesh_idx >= m_submeshes.size())
				mesh_idx = 0;
			return m_submeshes[mesh_idx].indices;
		}
		std::vector<unsigned long>& getIndices(size_t mesh_idx) {
			if (mesh_idx >= m_submeshes.size())
				mesh_idx = 0;
			return m_submeshes[mesh_idx].indices;
		}

	protected:
		// default material if submesh does not define one
		Material* m_material;
		
		std::vector<vertex> m_vertices;
		//std::vector<unsigned long> m_v_indices;
		std::vector<SubMesh> m_submeshes;
		bool m_isDynamic;
		VertexBuffer m_vertex;

		int m_state;

		// Copy Constructor 
		Mesh(const Mesh& m);
	};
}

#endif // !RHEA_MESH_H
