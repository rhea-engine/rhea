/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/ObjFile/ObjLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION 
#include <tiny_obj_loader/tiny_obj_loader.h>

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Resources/TextResource.h>
#include <Psio/Log.h>
#include <functional>
#include <Rhea/Core/Shader.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Resources/ImageResource.h>
#include <Rhea/Resources/VertexBufferStandard.h>

namespace rhea
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

    bool ObjLoader::load(rhea::Mesh* mesh)
    {
		std::vector<VertexAttribute> layout;

		std::string shader_name = getValueString("shadername", "//rhea/shaders/standard");
        Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(shader_name, m_attributes["basepath"], false));
        if (!shader_res) {
            LOG(LOG_ERROR) << "Could not find shader resource: " << shader_name;
            return false;
        }
		layout = shader_res->getVertexLayout();
		VertexBuffer* vertices = mesh->getVertexBuffer();        

        std::string objbuf;
        std::string mtlbuf;

        if (m_values.find("material") != m_values.end()) {
            TextResource* mtl = (TextResource*)m_app->getUnitTree()->find(m_values["material"].toString(), m_attributes["basepath"], false);
            if (mtl) {
                mtlbuf = mtl->getAsString();
            }
        }
        if (m_values.find("path") != m_values.end()) {
            TextResource* obj = (TextResource*)m_app->getUnitTree()->find(m_values["path"].toString(), m_attributes["basepath"], false);
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

        tinyobj::ObjReader reader;
        tinyobj::ObjReaderConfig config;
        bool change_handedness = true;

        bool ret = reader.ParseFromString(objbuf, mtlbuf, config);

        tinyobj::attrib_t attrib = reader.GetAttrib();
        std::vector<tinyobj::shape_t>shapes = reader.GetShapes();
        std::vector<tinyobj::material_t>materials = reader.GetMaterials();

        // prepare diffuse maps
		std::vector<ImageResource*> diffuse_maps;
        if (m_values.find("diffuse_tex") != m_values.end()) {
			Variant diffuse_tex = getValue("diffuse_tex");
            for (int i = 0; i < diffuse_tex.size(); i++) {               
                ImageResource* tex = doCast<ImageResource>(m_app->getUnitTree()->find(diffuse_tex[i].toString(), m_attributes["basepath"], false));
				diffuse_maps.push_back(tex);
            }
        }
		// prepare normal maps
		std::vector<ImageResource*> normal_maps;
		if (m_values.find("normal_tex") != m_values.end()) {
			Variant normal_tex = getValue("normal_tex");
			for (int i = 0; i < normal_tex.size(); i++) {
				ImageResource* tex = doCast<ImageResource>(m_app->getUnitTree()->find(normal_tex[i].toString(), m_attributes["basepath"], false));
				normal_maps.push_back(tex);
			}
		}

        std::vector<Material*> unit_materials;

        // create the materials
        int ambient_id = shader_res->getInputSlotId("AMBIENT");
		int diffuse_id = shader_res->getInputSlotId("DIFFUSE");
		int specular_id = shader_res->getInputSlotId("SPECULAR");
		int emmission_id = shader_res->getInputSlotId("EMISSIVE");

		int basecolor_id = shader_res->getTextureSlotId("BASECOLOR");
		int normal_id = shader_res->getTextureSlotId("NORMAL");

        for (int slot = 0; slot < materials.size(); slot++) {
			Material* mat = new Material(materials[slot].name);
			// set shader
            mat->setShader(shader_res);
            mat->setValue("shader", getValue("shadername", "standard"));
            
            // set material values
			mat->setColor(ambient_id, math::Color(materials[slot].ambient[0], materials[slot].ambient[1], materials[slot].ambient[2], 1.0f));
			mat->setColor(diffuse_id, math::Color(materials[slot].diffuse[0], materials[slot].diffuse[1], materials[slot].diffuse[2], 1.0f));
			mat->setColor(specular_id, math::Color(materials[slot].specular[0], materials[slot].specular[1], materials[slot].specular[2], 1.0f));
			mat->setColor(emmission_id, math::Color(materials[slot].emission[0], materials[slot].emission[1], materials[slot].emission[2], 1.0f));

            int shining_id = shader_res->getInputSlotId("SHININESS");
			// set shininess - map for Phong [0..1000.0] -> [0...128.0]
            float shininess = materials[0].shininess * 0.128f;
            mat->setFloat(shining_id, shininess);

			// set texture
            if (diffuse_maps.size() > slot && diffuse_maps[slot])
                mat->setTexture(basecolor_id, diffuse_maps[slot]);

			// set normal map
            if (normal_id != -1 && normal_maps.size() > slot) {
                mat->setTexture(normal_id, normal_maps[slot]);
            }
			
            // finalize
            mesh->addChild(mat);
			if (mesh->getMaterial() == nullptr) {
				mesh->setMaterial(mat);
			}
			unit_materials.push_back(mat);
        }

        std::unordered_map<Vertex, unsigned long, VertexKeyHasher> unique_vertices;

        // get vertex sizes
        int vertex_size = 0;
        int vertex_elements = 0;
        for (auto& attribute : layout) {
            vertex_size += attribute.size;
            vertex_elements += attribute.element_count;
        }

        vertices->setVertexSize(vertex_size);
        size_t buf_size = 0;
        // add buffer for one vertex with vertex_elements floats
        float* raw_vertex = new float[vertex_elements];

		// prepare indices
        std::vector<std::vector<uint32_t>> indices(materials.size());

        for (const auto& shape : shapes) {
            // shape name
            DLOG(LOG_DEBUG) << "Shape loaded: " << shape.name;

            // indent offset
            size_t index_offset = 0;

            // reserve buffer for vertex in number of vertices (assume triangles)
            buf_size += shape.mesh.num_face_vertices.size() * 3;
            vertices->reserve(buf_size);

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
                        unique_vertices[vertex] = static_cast<unsigned long>(vertices->size());
                        // iterate iver vertex attributes and add to vertex buffer
                        size_t idx = 0;
                        if (shader_res->getValue("input", "") == "standard") {
                            VertexBufferStandard::VertexStandard v;
                            v.position = vertex.position;
                            v.normal = vertex.normal;
                            v.texcoord = vertex.uv;
                            v.tangent = { 0.0f, 0.0f, 0.0f };

							((VertexBufferStandard*)vertices)->push_back(v);

                            /*


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

                            vertices->push_back(raw_vertex);
                            */                        
                        }
                    }
					if (shape.mesh.material_ids[f] >= indices.size()) {
						LOG(LOG_ERROR) << "Material id " << shape.mesh.material_ids[f] << " is out of range for shape " << shape.name;
						return false;
					}
                    indices[shape.mesh.material_ids[f]].push_back(unique_vertices[vertex]);
                }

                index_offset += num_vertices;
                // per-face material
                int face_id = shape.mesh.material_ids[f];
            }

            if (change_handedness) {
                for (size_t m = 0; m < materials.size(); m++)
                    for (size_t i = 0; i < indices[m].size(); i += 3) {
                        std::swap(indices[m][i + 1], indices[m][i + 2]);
                    }
            }

            if (shader_res->getValue("input", "") == "standard") {

				// compute tangents
                for (size_t i = 0; i < materials.size(); i++)
                {
                    // iterate over faces
                    for (size_t face = 0; face < indices[i].size(); face += 3)
                    {
                        size_t idx1 = indices[i][face];
                        size_t idx2 = indices[i][face + 1];
                        size_t idx3 = indices[i][face + 2];

                        VertexBufferStandard::VertexStandard* v1 = (*(VertexBufferStandard*)vertices)[idx1];
						VertexBufferStandard::VertexStandard* v2 = (*(VertexBufferStandard*)vertices)[idx2];
						VertexBufferStandard::VertexStandard* v3 = (*(VertexBufferStandard*)vertices)[idx3];

                        math::Vector3 edge1 = v2->position - v1->position;
                        math::Vector3 edge2 = v3->position - v1->position;
                        math::Vector2 deltaUV1 = v2->texcoord - v1->texcoord;
                        math::Vector2 deltaUV2 = v3->texcoord - v1->texcoord;

                        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                        math::Vector3 tangent;

                        tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * f;

						v1->tangent += tangent;
						v2->tangent += tangent;
						v3->tangent += tangent;
                    }
                }

                for (unsigned int idx = 0; idx < vertices->size(); idx++) {
                    VertexBufferStandard::VertexStandard* v = (*(VertexBufferStandard*)vertices)[idx];
                    v->tangent.normalize();
                }
            }

            // store submeshes
            for (size_t i = 0; i < materials.size(); i++)
                mesh->setSubmesh(i, unit_materials[i], indices[i]);
        }
        // delete raw vertex
        delete[] raw_vertex;

        return ret;
    }

    std::vector<Unit::ResourceReference> ObjLoader::getReferences()
    {
        std::vector<Unit::ResourceReference> refs;
        if (m_values.find("path") != m_values.end())
            refs.push_back({ "obj", "//meworla/wavefront-object", m_values["path"] });
        else
            LOG(LOG_ERROR) << "Wavefront Obj-Loader: no path to obj file defined";
        if (m_values.find("material") != m_values.end())
            refs.push_back({ "material", "//meworla/wavefront-material", m_values["material"] });
        return refs;
    }
}
