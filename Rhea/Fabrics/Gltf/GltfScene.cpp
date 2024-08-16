#include "GltfScene.h"
#include <Core/Shader.h>
#include <Psio/Log.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Resources/VertexBufferStandard.h>
#include <Rhea/Fabrics/Media/ImageResource.h>
#include <Rhea/Fabrics/Core/Material.h>

namespace Rhea
{
	bool GltfScene::load(Rhea::Mesh* mesh)
	{
		// instanced?
		if (m_instance)
			return m_instance->load(mesh);

		// check if we have at least one scene
		if (!m_model.scenes.size())
		{
			return false;
		}

		// get the default shader
		std::string shader_name = mesh->getValue("shadername", "//rhea/shaders/standard");
		Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(shader_name, m_attributes["basepath"], false));
		if (!shader_res) {
			LOG(LOG_ERROR) << "Could not find shader resource: " << shader_name;
			return false;
		}

		// prepare vertex buffer
		mesh->getVertexBuffer()->setVertexSize(shader_res->getVertexSize());

		// prepare materials
		int ambient_id = shader_res->getInputSlotId("AMBIENT");
		int diffuse_id = shader_res->getInputSlotId("DIFFUSE");
		int specular_id = shader_res->getInputSlotId("SPECULAR");
		int emmission_id = shader_res->getInputSlotId("EMISSIVE");

		int basecolor_id = shader_res->getTextureSlotId("BASECOLOR");
		int normal_id = shader_res->getTextureSlotId("NORMAL");

		if (m_app)
			m_app = mesh->getApplication();

		// prepare diffuse maps (from mesh options)
		std::vector<ImageResource*> diffuse_maps;
		Variant diffuse_tex = mesh->getValue("diffuse_tex");
		if (diffuse_tex.isArray()) {
			for (int i = 0; i < diffuse_tex.size(); i++) {
				ImageResource* tex = doCast<ImageResource>(m_app->getUnitTree()->find(diffuse_tex[i], mesh->getBasePath(), false));
				diffuse_maps.push_back(tex);
			}
		}
		// prepare normal maps (from mesh options)
		std::vector<ImageResource*> normal_maps;
		Variant normal_tex = mesh->getValue("normal_tex");
		if (normal_tex.isArray()) {
			for (int i = 0; i < normal_tex.size(); i++) {
				ImageResource* tex = doCast<ImageResource>(m_app->getUnitTree()->find(normal_tex[i], m_attributes["basepath"], false));
				normal_maps.push_back(tex);
			}
		}
		Variant override_materials = mesh->getValue("material");

		if (override_materials.isArray()) {
			// TODO make overriding of material possible
			for (int i = 0; i < override_materials.size(); i++) {
				Material* mat = doCast<Material>(m_app->getUnitTree()->find(override_materials[i], mesh->getBasePath(), false));
				// note: if mat is null the default material will be used
				m_materials.push_back(mat);
			}
		}
		else if (m_model.materials.size() > 0) {

			// create the materials from gltf file
			for (const auto& material : m_model.materials) {
				Material* mat = new Material(material.name);
				// set shader
				mat->setShaderRes(shader_res);
				mat->setValue("shader", shader_name);

				// set material values

				// traditional shader - fill as good as possible
				// pbr to traditional ambient - not implemented
				//mat->setColor(ambient_id, Math::Vector3(material.ambient[0], material.ambient[1], material.ambient[2]));
				// pbr to traditional specular - not implemented
				//mat->setColor(specular_id, Math::Vector3(material.specular[0], material.specular[1], material.specular[2]));
				Vector3 diffuse(
					(float)material.pbrMetallicRoughness.baseColorFactor[0],
					(float)material.pbrMetallicRoughness.baseColorFactor[1],
					(float)material.pbrMetallicRoughness.baseColorFactor[2]
				);
				mat->setColor(diffuse_id, diffuse);
				Vector3 emmissive(
					(float)material.emissiveFactor[0],
					(float)material.emissiveFactor[1],
					(float)material.emissiveFactor[2]
				);
				mat->setColor(emmission_id, emmissive);

				// file baseColor texture
				if (material.pbrMetallicRoughness.baseColorTexture.index >= 0) {
					int textureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
					if (textureIndex > -1) {
						// try override
						if (diffuse_maps.size() > textureIndex)
							mat->setTexture(basecolor_id, diffuse_maps[textureIndex]);
						else {
							auto tex = m_images.find(textureIndex);
							if (tex != m_images.end()) {
								mat->setTexture(basecolor_id, tex->second);
							}
						}
					}
				}
				// fill normal maps (if any)
				int normalIndex = material.normalTexture.index;
				if (normalIndex > -1) {
					// try override
					if (normal_maps.size() > normalIndex)
						mat->setTexture(normal_id, normal_maps[normalIndex]);
					else {
						auto tex = m_images.find(normalIndex);
						if (tex != m_images.end()) {
							mat->setTexture(normal_id, tex->second);
						}
					}
				}

				addChild(mat);
				m_materials.push_back(mat);
			}
		}
		else {
			// no materials were defined - neither in gltf file nor in mesh options
			// add a default material
			Material* mat = new Material("default");
			// set shader
			mat->setShaderRes(shader_res);
			mat->setValue("shader", shader_name);

			mat->setColor(diffuse_id, Vector3(0.8f, 0.8f, 0.8f));
			mat->setColor(emmission_id, Vector3(0.0f, 0.0f, 0.0f));

			if (diffuse_maps.size() > 0)
				mat->setTexture(basecolor_id, diffuse_maps[0]);
			if (normal_maps.size() > 0)
				mat->setTexture(normal_id, normal_maps[0]);

			addChild(mat);
			m_materials.push_back(mat);
		}

		// if default scene is defined use it - else use first scene
		int default_scene = m_model.defaultScene > -1 ? m_model.defaultScene : 0;
		const tinygltf::Scene& scene = m_model.scenes[default_scene];
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			tinygltf::Node node = m_model.nodes[scene.nodes[i]];
			HandleNode(node, mesh);
			//m_model.meshes[node.mesh].primitives[0].attributes[0]
		}

		return false;
	}

	void GltfScene::ready()
	{
		if (m_instance_path != "") {
			m_instance = doCast<GltfScene>(m_app->getUnitTree()->find(m_instance_path, m_attributes["basepath"], false));
			if (!m_instance)
				LOG(LOG_ERROR) << "Could not find Gltf scene at " << m_instance_path << " (basepath=" << m_attributes["basepath"] << ")";
			else {
				// copy the children
				m_instance->Instantiate(this);
			}
		}
	}

	std::vector<Unit::Export> GltfScene::getReferences()
	{
		std::vector<Export> refs;
		if (m_values.find("path") != m_values.end()) {
			refs.push_back({ "path", "//meworla/gltf+json", m_values["path"] });
		}
		return refs;
	}

	void GltfScene::Instantiate(Unit* owner)
	{
		for (auto it = m_ordered_children.begin(); it != m_ordered_children.end(); it++)
		{
			Unit* child = (*it)->Clone();
			owner->addChild(child);
		}
	}

	std::vector<unsigned long> GltfScene::ExtractIndices(const tinygltf::Accessor& accessor) 
	{
		std::vector<unsigned long> indices;

		// Get the buffer view from the accessor
		const tinygltf::BufferView& bufferView = m_model.bufferViews[accessor.bufferView];

		// Get the actual buffer data
		const tinygltf::Buffer& buffer = m_model.buffers[bufferView.buffer];

		// Determine the type of indices
		const unsigned char* dataPtr = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
		size_t indexCount = accessor.count;

		// Fill the indices vector depending on the component type
		switch (accessor.componentType) {
		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
			for (size_t i = 0; i < indexCount; ++i) {
				indices.push_back(static_cast<unsigned long>(dataPtr[i]));
			}
			break;
		}
		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
			const unsigned short* indicesPtr = reinterpret_cast<const unsigned short*>(dataPtr);
			for (size_t i = 0; i < indexCount; ++i) {
				indices.push_back(static_cast<unsigned long>(indicesPtr[i]));
			}
			break;
		}
		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
			const unsigned int* indicesPtr = reinterpret_cast<const unsigned int*>(dataPtr);
			for (size_t i = 0; i < indexCount; ++i) {
				indices.push_back(static_cast<unsigned long>(indicesPtr[i]));
			}
			break;
		}
		default:
			LOG(LOG_ERROR) << "GltfScene::ExtractIndices: Unsupported index component type.";
		}

		return indices;
	}

	void GltfScene::HandleNode(tinygltf::Node& node, Mesh* u_mesh)
	{
		LOG(LOG_DEBUG) << "Loading node: " << node.name;

		// if we have a mesh add it as submesh
		if (node.mesh > -1) {
			tinygltf::Mesh& mesh = m_model.meshes[node.mesh];
			for (size_t i = 0; i < mesh.primitives.size(); i++) {
				const tinygltf::Primitive& primitive = mesh.primitives[i];
				if (primitive.indices < 0) break;

				VertexBuffer* vertices = u_mesh->getVertexBuffer();

				Vertex vertex = {};

				const float* positions = nullptr;
				const float* normals = nullptr;
				const float* texcoords = nullptr;
				size_t vertex_count = 0;

				// position

				if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
					int position_index = primitive.attributes.find("POSITION")->second;
					const tinygltf::Accessor& pos_accessor = m_model.accessors[position_index];
					const tinygltf::BufferView& pos_bufferView = m_model.bufferViews[pos_accessor.bufferView];
					const tinygltf::Buffer& pos_buffer = m_model.buffers[pos_bufferView.buffer];

					// Assuming 3D positions
					positions = reinterpret_cast<const float*>(&pos_buffer.data[pos_accessor.byteOffset + pos_bufferView.byteOffset]);

					vertex_count = pos_accessor.count; // number of vertices in position attribute
				}
				else
					break;

				// normals

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					int normal_index = primitive.attributes.find("NORMAL")->second;
					const tinygltf::Accessor& normal_accessor = m_model.accessors[normal_index];
					const tinygltf::BufferView& normal_bufferView = m_model.bufferViews[normal_accessor.bufferView];
					const tinygltf::Buffer& normal_buffer = m_model.buffers[normal_bufferView.buffer];

					normals = reinterpret_cast<const float*>(&normal_buffer.data[normal_accessor.byteOffset + normal_bufferView.byteOffset]);
				}

				// texcoords

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
					int texcoord_index = primitive.attributes.find("TEXCOORD_0")->second;
					const tinygltf::Accessor& texcoord_accessor = m_model.accessors[texcoord_index];
					const tinygltf::BufferView& texcoord_bufferView = m_model.bufferViews[texcoord_accessor.bufferView];
					const tinygltf::Buffer& texcoord_buffer = m_model.buffers[texcoord_bufferView.buffer];

					texcoords = reinterpret_cast<const float*>(&texcoord_buffer.data[texcoord_accessor.byteOffset + texcoord_bufferView.byteOffset]);
				}

				for (int j = 0; j < vertex_count; j++) {
					vertex.position.x = positions[j * 3 + 0];
					vertex.position.y = positions[j * 3 + 1];
					vertex.position.z = positions[j * 3 + 2];

					if (normals) {
						vertex.normal.x = normals[j * 3 + 0];
						vertex.normal.y = normals[j * 3 + 1];
						vertex.normal.z = normals[j * 3 + 2];
					}

					if (texcoords) {
						vertex.uv.x = texcoords[j * 2 + 0];
						vertex.uv.y = texcoords[j * 2 + 1];
					}

					VertexBufferStandard::VertexStandard v;
					v.position = vertex.position;
					v.normal = vertex.normal;
					v.texcoord = vertex.uv;
					v.tangent = { 0.0f, 0.0f, 0.0f };
					//vertices->push_back((float*)&v);
					((VertexBufferStandard*)vertices)->push_back(v);
				}

				// indices
				if (primitive.indices >= 0) {
					const tinygltf::Accessor& index_accessor = m_model.accessors[primitive.indices];

					Material* mat = nullptr;
					if (m_materials.size() > primitive.material)
						mat = m_materials[primitive.material];
					if (!mat)
						// if no material found use default
						mat = u_mesh->getDefaultMaterial();
					if (!mat && m_materials.size() > 0)
						// still no material - try index 0 for default material
						mat = m_materials[0];

					u_mesh->addSubmesh(mat, ExtractIndices(index_accessor));

					//u_mesh->addSubmesh(def_mat, m_model.meshes[node.mesh].primitives[0].indices);
				}
			}
		}	

		// Handle child nodes.
		for (size_t i = 0; i < node.children.size(); i++) {
			HandleNode(m_model.nodes[node.children[i]], u_mesh);
		}
	}
}