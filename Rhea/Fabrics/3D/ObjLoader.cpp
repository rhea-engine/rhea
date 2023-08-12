#define TINYOBJLOADER_IMPLEMENTATION 
#include <tiny_obj_loader/tiny_obj_loader.h>

#include "ObjLoader.h"
#include <Application.h>
#include <UnitTree.h>
#include <Resources/TextResource.h>
#include <Psio/Log.h>

namespace Rhea
{
    struct VertexKeyHasher
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

	bool ObjLoader::load(std::string objbuf, std::string mtlbuf, std::vector<Mesh::vertex>& vertices, std::vector<unsigned long>& indices)
	{
		tinyobj::ObjReader reader;
		tinyobj::ObjReaderConfig config;     
        bool change_handedness = true;
		
		bool ret = reader.ParseFromString(objbuf, mtlbuf, config);

		tinyobj::attrib_t attrib = reader.GetAttrib();
        std::vector<tinyobj::shape_t>shapes = reader.GetShapes();
        std::vector<tinyobj::material_t>materials = reader.GetMaterials();

        std::unordered_map<Mesh::vertex, unsigned long, VertexKeyHasher> unique_vertices;

        for (const auto& shape : shapes) {
            // shape name
            DLOG(LOG_DEBUG) << "Shape loaded: " << shape.name;
            
            // indent offset
            size_t index_offset = 0;
            
            // iterate over faces
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {

                // iterate over vertices composing face f
                int num_vertices = shape.mesh.num_face_vertices[f];
                for (size_t v = 0; v < num_vertices; v++) {

                    Mesh::vertex vertex;

                    // access to vertex
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                    // vertex coordinates
                    vertex.x = attrib.vertices[3 * idx.vertex_index + 0];
                    vertex.y = attrib.vertices[3 * idx.vertex_index + 1];
                    vertex.z = attrib.vertices[3 * idx.vertex_index + 2];

                    // right hand model ?
                    if (change_handedness) {
                        vertex.z *= -1.0f;
                    }

                    // vertex normal
                    float nx = attrib.normals[3 * idx.normal_index + 0];
                    float ny = attrib.normals[3 * idx.normal_index + 1];
                    float nz = attrib.normals[3 * idx.normal_index + 2];

                    // vertex texture coordinates
                    vertex.u = attrib.texcoords[2 * idx.texcoord_index + 0];
                    vertex.v = attrib.texcoords[2 * idx.texcoord_index + 1];

                    // flip v coordinate
                    if (change_handedness)
                        vertex.v = 1.0f - vertex.v;

                    // vertex color
                    float red = attrib.colors[3 * idx.vertex_index + 0];
                    float green = attrib.colors[3 * idx.vertex_index + 1];
                    float blue = attrib.colors[3 * idx.vertex_index + 2];

                    if (unique_vertices.count(vertex) == 0)
                    {
                        // new vertex - store current position
                        unique_vertices[vertex] = static_cast<unsigned long>(vertices.size());
                        vertices.push_back(vertex);
                    }

                    indices.push_back(unique_vertices[vertex]);
                }

                index_offset += num_vertices;
                // per-face material
                shape.mesh.material_ids[f];
            }
            
            if (change_handedness) {
                for (size_t i = 0; i < indices.size(); i += 3) {
                    std::swap(indices[i + 1], indices[i + 2]);
                }
            }
        }

		return ret;
	}

    bool ObjLoader::get(std::vector<Mesh::vertex>& vertices, std::vector<unsigned long>& indices)
    {
        std::string objbuf;
        std::string mtlbuf;

        if (m_values.find("material") != m_values.end()) {
            TextResource* mtl = (TextResource*)m_app->getUnitTree()->find(m_values["material"]);
            if (mtl) {
                mtlbuf = mtl->getAsString();
            }
        }
        if (m_values.find("path") != m_values.end()) {
            TextResource* obj = (TextResource*)m_app->getUnitTree()->find(m_values["path"]);
            if (obj) {
                objbuf = obj->getAsString();
            }
            else {
                DLOG(LOG_DEBUG) << "obj not found in objloader";
                return false;
            }
        }
        else {
            LOG(LOG_ERROR) << "Could not open " << m_pathname << ": Obj file not defined.";
            return false;
        }
        
        return load(objbuf, mtlbuf, vertices, indices);
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
