#define TINYOBJLOADER_IMPLEMENTATION 
#include <tiny_obj_loader/tiny_obj_loader.h>
#include "Poseidon/Mesh.h"

#include "Importer.h"

namespace Rhea
{
	bool Importer::load(Application* app, std::string objbuf, std::string mtlbuf, std::vector<Hippios::vertex_uv>& vertices, std::vector<unsigned long>& indices)
	{
		tinyobj::ObjReader reader;
		tinyobj::ObjReaderConfig config; 

        //std::vector<Hippios::vertex_uv> vertices;
		
		bool ret = reader.ParseFromString(objbuf, mtlbuf, config);

		tinyobj::attrib_t attrib = reader.GetAttrib();
        std::vector<tinyobj::shape_t>shapes = reader.GetShapes();
        std::vector<tinyobj::material_t>materials = reader.GetMaterials();

        for (const auto& shape : shapes) {
            // shape name
            printf("Shape: %s\n", shape.name.c_str());
            size_t index_offset = 0;// indent offset
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                // number of vertices composing face f
                int num_vertices = shape.mesh.num_face_vertices[f];
                for (size_t v = 0; v < num_vertices; v++) {

                    Hippios::vertex_uv vertex;

                    // access to vertex
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    // vertex coordinates
                    vertex.x = attrib.vertices[3 * idx.vertex_index + 0];
                    vertex.y = attrib.vertices[3 * idx.vertex_index + 1];
                    vertex.z = attrib.vertices[3 * idx.vertex_index + 2];
                    // vertex normal
                    float nx = attrib.normals[3 * idx.normal_index + 0];
                    float ny = attrib.normals[3 * idx.normal_index + 1];
                    float nz = attrib.normals[3 * idx.normal_index + 2];
                    // vertex texture coordinates
                    vertex.u =
                        attrib.texcoords[2 * idx.texcoord_index + 0];
                    vertex.v =
                        attrib.texcoords[2 * idx.texcoord_index + 1];
                    // vertex color
                    float red = attrib.colors[3 * idx.vertex_index + 0];
                    float green = attrib.colors[3 * idx.vertex_index + 1];
                    float blue = attrib.colors[3 * idx.vertex_index + 2];

                    vertices.push_back(vertex);
                    indices.push_back(f*3 + v);
                }
                index_offset += num_vertices;
                // per-face material
                shape.mesh.material_ids[f];
            }
        }

		return ret;
	}
}
