#include "GltfScene.h"
#include <Core/Shader.h>
#include <Psio/Log.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Resources/VertexBufferStandard.h>

namespace Rhea
{
	bool GltfScene::load(Rhea::Mesh* mesh)
	{
		// check if we have at least one scene
		if (!m_model.scenes.size())
		{
			return false;
		}

		// prepare vertex buffer
		std::vector<VertexAttribute> layout;

		std::string shader_name = mesh->getValue("shadername", "//rhea/shaders/standard");
		Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(shader_name, m_attributes["basepath"], false));
		if (!shader_res) {
			LOG(LOG_ERROR) << "Could not find shader resource: " << shader_name;
			return false;
		}
		layout = shader_res->getVertexLayout();
		VertexBuffer* vertices = mesh->getVertexBuffer();

		// get vertex sizes
		int vertex_size = 0;
		int vertex_elements = 0;
		for (auto& attribute : layout) {
			vertex_size += attribute.size;
			vertex_elements += attribute.element_count;
		}

		vertices->setVertexSize(vertex_size);

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
		}
	}

	std::vector<Unit::Export> GltfScene::getReferences()
	{
		std::vector<Export> refs;
		if (m_values.find("path") != m_values.end()) {
			refs.push_back({ "path", "//meworla/gltf+json", m_values["path"] });
			m_instance_path = m_values["path"];
		}
		return refs;
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

					const float* texcoords = reinterpret_cast<const float*>(&texcoord_buffer.data[texcoord_accessor.byteOffset]);
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
				const tinygltf::Accessor& index_accessor = m_model.accessors[primitive.indices];
			}

			//mesh->addSubmesh(m_model.meshes[node.mesh].primitives[0].material, m_model.meshes[node.mesh].primitives[0].indices);

		}	

		// Handle child nodes.
		for (size_t i = 0; i < node.children.size(); i++) {
			LOG(LOG_DEBUG) << "Node: " << node.children[i];
			HandleNode(m_model.nodes[node.children[i]], u_mesh);
		}
	}
}