#define TINYOBJLOADER_IMPLEMENTATION 
#include <tiny_obj_loader/tiny_obj_loader.h>

#include "ObjLoader.h"
#include <Application.h>
#include <UnitTree.h>
#include <Resources/TextResource.h>
#include <Psio/Log.h>
#include <functional>

namespace Rhea
{
    struct VertexKeyHasher {
        size_t operator()(const ObjLoader::Vertex& v) const {
            size_t hash = 17;
            hash = hash * 31 + std::hash<float>{}(v.position.x);
            hash = hash * 31 + std::hash<float>{}(v.position.y);
            hash = hash * 31 + std::hash<float>{}(v.position.z);
          //  hash = hash * 31 + std::hash<float>{}(v.normal.x);
           // hash = hash * 31 + std::hash<float>{}(v.normal.y);
          //  hash = hash * 31 + std::hash<float>{}(v.normal.z);
          //  hash = hash * 31 + std::hash<float>{}(v.color.x);
          //  hash = hash * 31 + std::hash<float>{}(v.color.y);
          //  hash = hash * 31 + std::hash<float>{}(v.color.z);
            hash = hash * 31 + std::hash<float>{}(v.uv.x);
            hash = hash * 31 + std::hash<float>{}(v.uv.y);
            return hash;
        }
    };    

    struct VertexKeyHasherUV
    {
        std::size_t operator()(const Mesh::vertex& v) const
        {
            using std::size_t;
            using std::hash;
            using std::string;

            size_t res = 17;
            res = res * 31 + hash<float>()(v.x);
            res = res * 31 + hash<float>()(v.y);
            res = res * 31 + hash<float>()(v.z);
            res = res * 31 + hash<float>()(v.u);
            res = res * 31 + hash<float>()(v.v);
            return res;
        }
    };

	bool ObjLoader::load(std::string objbuf, std::string mtlbuf, std::vector<VertexAttribute>& layout, VertexBuffer& vertices, std::vector<unsigned long>& indices)
	{
		tinyobj::ObjReader reader;
		tinyobj::ObjReaderConfig config;     
        bool change_handedness = true;
		
		bool ret = reader.ParseFromString(objbuf, mtlbuf, config);

		tinyobj::attrib_t attrib = reader.GetAttrib();
        std::vector<tinyobj::shape_t>shapes = reader.GetShapes();
        std::vector<tinyobj::material_t>materials = reader.GetMaterials();

        std::unordered_map<Vertex, unsigned long, VertexKeyHasher> unique_vertices;

        // get vertex sizes
		int vertex_size = 0;
		int vertex_elements = 0;
		for (auto& attribute : layout) {
			vertex_size += attribute.size;
			vertex_elements += attribute.element_count;
		}

		vertices.setVertexSize(vertex_size);
        size_t buf_size = 0;
        // add buffer for one vertex with vertex_elements floats
		float* raw_vertex = new float[vertex_elements];

        for (const auto& shape : shapes) {
            // shape name
            DLOG(LOG_DEBUG) << "Shape loaded: " << shape.name;
            
            // indent offset
            size_t index_offset = 0;

			// reserve buffer for vertex in number of vertices (assume triangles)
            buf_size += shape.mesh.num_face_vertices.size() * 3;
			vertices.reserve(buf_size);
            
            // iterate over faces
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {

                // iterate over vertices composing face f
                int num_vertices = shape.mesh.num_face_vertices[f];
                for (size_t v = 0; v < num_vertices; v++) {

                    Vertex vertex = {};

                    // access to vertex
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    
                    // vertex coordinates
                    vertex.position.x = attrib.vertices[3 * idx.vertex_index + 0];
                    vertex.position.y = attrib.vertices[3 * idx.vertex_index + 1];
                    vertex.position.z = attrib.vertices[3 * idx.vertex_index + 2];

                    // right hand model ?
                    if (change_handedness) {
                        vertex.position.z *= -1.0f;
                    }

                    // vertex normal
                    vertex.normal.x = attrib.normals[3 * idx.normal_index + 0];
                    vertex.normal.y = attrib.normals[3 * idx.normal_index + 1];
                    vertex.normal.z = attrib.normals[3 * idx.normal_index + 2];

                    // vertex texture coordinates
                    vertex.uv.x = attrib.texcoords[2 * idx.texcoord_index + 0];
                    vertex.uv.y = attrib.texcoords[2 * idx.texcoord_index + 1];

                    // flip v coordinate
                    if (change_handedness)
                        vertex.uv.y = 1.0f - vertex.uv.y;

                    // vertex color
                    vertex.color.x = attrib.colors[3 * idx.vertex_index + 0]; // red
                    vertex.color.y = attrib.colors[3 * idx.vertex_index + 1]; // green
                    vertex.color.z = attrib.colors[3 * idx.vertex_index + 2]; // blue
					
                    if (unique_vertices.count(vertex) == 0)
                    {
                        // new vertex - store current position
                        unique_vertices[vertex] = static_cast<unsigned long>(vertices.size());
						// iterate iver vertex attributes and add to vertex buffer
                        size_t idx = 0;
                        for (auto& attribute : layout) {
                            switch (attribute.type) {
                            case VertexAttributeType::POSITION:
                                raw_vertex[idx++] = vertex.position.x;
                                raw_vertex[idx++] = vertex.position.y;
                                raw_vertex[idx++] = vertex.position.z;
                                break;
							case VertexAttributeType::NORMAL:
                                raw_vertex[idx++] = vertex.normal.x;
                                raw_vertex[idx++] = vertex.normal.y;
                                raw_vertex[idx++] = vertex.normal.z;
                                break;
							case VertexAttributeType::TEXCOORD:
                                raw_vertex[idx++] = vertex.uv.x;
                                raw_vertex[idx++] = vertex.uv.y;
                                break;
							case VertexAttributeType::COLOR:
                                raw_vertex[idx++] = vertex.color.x;
                                raw_vertex[idx++] = vertex.color.y;
                                raw_vertex[idx++] = vertex.color.z;
                                break;
                            }
						}                    

                        vertices.push_back(raw_vertex);
                    }

                    indices.push_back(unique_vertices[vertex]);
                }

                index_offset += num_vertices;
                // per-face material
                int face_id = shape.mesh.material_ids[f];
            }
            
            if (change_handedness) {
                for (size_t i = 0; i < indices.size(); i += 3) {
                    std::swap(indices[i + 1], indices[i + 2]);
                }
            }
        }
		// delete raw vertex
		delete[] raw_vertex;

		return ret;
	}

    bool ObjLoader::get(std::vector<VertexAttribute>& layout, VertexBuffer& vertices, std::vector<unsigned long>& indices)
    {
        std::string objbuf;
        std::string mtlbuf;

        if (m_values.find("material") != m_values.end()) {
            TextResource* mtl = (TextResource*)m_app->getUnitTree()->find(m_values["material"], m_attributes["basepath"], false);
            if (mtl) {
                mtlbuf = mtl->getAsString();
            }
        }
        if (m_values.find("path") != m_values.end()) {
            TextResource* obj = (TextResource*)m_app->getUnitTree()->find(m_values["path"], m_attributes["basepath"], false);
            if (obj) {
                objbuf = obj->getAsString();
            }
            else {
                DLOG(LOG_DEBUG) << "Obj not found in objloader. Path was: " << (std::string)m_values["path"];
                return false;
            }
        }
        else {
            LOG(LOG_ERROR) << "Could not open " << m_pathname << ": Obj file not defined.";
            return false;
        }
        
        return load(objbuf, mtlbuf, layout, vertices, indices);
    }

    std::vector<Unit::Export> ObjLoader::getReferences()
    {
        std::vector<Unit::Export> refs;
        if (m_values.find("path") != m_values.end())
            refs.push_back({ "obj", "//meworla/wavefront-object", m_values["path"] });
        else
            LOG(LOG_ERROR) << "Wavefront Obj-Loader: no path to obj file defined";
        if (m_values.find("material") != m_values.end())
            refs.push_back({ "material", "//meworla/wavefront-material", m_values["material"] });
        return refs;
    }
}
