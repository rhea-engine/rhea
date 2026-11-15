/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Gltf/GltfDocument.h>
#include <Rhea/Core/Shader.h>
#include <Psio/Log.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Resources/VertexBufferStandard.h>
#include <Rhea/Resources/VertexBufferSkeletal.h>
#include <Rhea/Resources/ImageResource.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Animation/Animation.h>
#include <Rhea/Animation/Skeleton.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <Rhea/Core/SceneDefinitionComponent.h>
#undef max
#include <algorithm>
#include <unordered_set>

#define MAX_BONES 100

namespace rhea
{
	bool GltfDocument::load(rhea::Mesh* mesh, int mesh_index, int skin_index)
	{
		// instanced?
		if (m_instance)
			return m_instance->load(mesh, mesh_index, skin_index);

		if (!m_app)
			m_app = mesh->getApplication();

		// check if we have at least one scene
		if (!m_model.scenes.size())
		{
			return false;
		}

		if (!m_static_materials.created) {
			if (!createMaterials(false))
				return false;
			m_static_materials.created = true;
		}
		if (!m_skinned_materials.created) {
			if (!createMaterials(true))
				return false;
			m_skinned_materials.created = true;
		}
		if (mesh_index >= 0)
			return importMesh(mesh_index, skin_index, mesh);

		// Existing geometry behavior: import all nodes.
		
		// if default scene is defined use it - else use first scene
		int default_scene = m_model.defaultScene > -1 ? m_model.defaultScene : 0;
		const tinygltf::Scene& scene = m_model.scenes[default_scene];
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			tinygltf::Node node = m_model.nodes[scene.nodes[i]];
			HandleNode(node, mesh);
			//m_model.meshes[node.mesh].primitives[0].attributes[0]
		}

		return true;
	}



	bool GltfDocument::buildSceneDefinition(SceneResource::SceneDefinition& definition)
	{
		definition = {};
		std::unordered_set<std::string> used_ids;

		const int scene_index = m_model.defaultScene >= 0 ? m_model.defaultScene : 0;

		if (static_cast<size_t>(scene_index) >= m_model.scenes.size())
			return false;

		const auto& source_scene = m_model.scenes[scene_index];

		definition.root.name = "gltf_scene";
		definition.root.type_name = "//meworla/item"; // TODO: scene or item?

		std::function<bool(int, const std::string&)> append_node;
		append_node = [&](int node_index, const std::string& parent_id) {
			if (node_index < 0 ||
				static_cast<size_t>(node_index) >= m_model.nodes.size())
				return false;

			const auto& source = m_model.nodes[node_index];			

			// Index ensures unique names even for unnamed/duplicate nodes.
			std::string base_name = source.name.empty() ? "node_" + std::to_string(node_index) : source.name;
			// replace invalid characters in base_name with '_'
			std::replace_if(base_name.begin(), base_name.end(),	[](char c) { return c == '/' || c == '\\'; }, '_');
			if (base_name == "." || base_name == "..")
				base_name = "node_" + std::to_string(node_index);
			std::string node_id = parent_id.empty() ? base_name : parent_id + "/" + base_name;
			int suffix_index = 1;
			std::string name = base_name;
			while (used_ids.find(node_id) != used_ids.end()) {
				name = base_name + "_" + std::to_string(suffix_index++);
				node_id = parent_id.empty() ? name : parent_id + "/" + name;
			}
			used_ids.insert(node_id);
			// Reserve the generated component's path as well.
			used_ids.insert(node_id + "/transform");			
			used_ids.insert(node_id + "/renderer");

			SceneResource::NodeDefinition node;
			node.node_id = node_id;
			node.parent_id = parent_id;
			node.name = name;
			node.type_name = "//meworla/item";
			if (source.mesh >= 0) {
				Mesh* mesh = getOrCreateMesh(source.mesh, source.skin);
				if (!mesh)
					return false;

				// Store the mesh path in the node values for later use.
				node.values["mesh"] = mesh->getPath();
			}
			definition.nodes.push_back(std::move(node));

			SceneResource::NodeDefinition transform;
			transform.node_id = node_id + "/transform";
			transform.parent_id = node_id;
			transform.name = "transform";
			transform.type_name = "//meworla/transform";

			if (!source.matrix.empty()) {
				math::Vector3 translation;
				math::Quaternion rotation;
				math::Vector3 scale;

				if (source.matrix.size() != 16 || !decomposeTRS(math::Matrix4(source.matrix.data()), translation, rotation, scale)) {
					LOG(LOG_ERROR) << "Failed to decompose matrix for node " << node_id;
					return false;
				}
				transform.values["position"] = std::vector<Variant>{
					float(translation.x),
					float(translation.y),
					float(translation.z)
				};
				transform.values["rotation"] = std::vector<Variant>{
					float(rotation.w),
					float(rotation.x),
					float(rotation.y),
					float(rotation.z)
				};
				transform.values["scale"] = std::vector<Variant>{
					float(scale.x),
					float(scale.y),
					float(scale.z)
				};
			}
			else {
				if (source.translation.size() == 3) {
					transform.values["position"] = std::vector<Variant>{
						float(source.translation[0]),
						float(source.translation[1]),
						float(source.translation[2])
					};
				}

				if (source.rotation.size() == 4) {
					// glTF: x,y,z,w; Rhea Quaternion: w,x,y,z.
					transform.values["rotation"] = std::vector<Variant>{
						float(source.rotation[3]),
						float(source.rotation[0]),
						float(source.rotation[1]),
						float(source.rotation[2])
					};
				}

				if (source.scale.size() == 3) {
					transform.values["scale"] = std::vector<Variant>{
						float(source.scale[0]),
						float(source.scale[1]),
						float(source.scale[2])
					};
				}
			}

			definition.nodes.push_back(std::move(transform));

			for (int child_index : source.children) {
				if (!append_node(child_index, node_id))
					return false;
			}

			return true;
			};

		for (int root_index : source_scene.nodes) {
			if (!append_node(root_index, ""))
				return false;
		}

		return true;
	}

	Mesh* GltfDocument::getOrCreateMesh(int mesh_index, int skin_index)
	{
		const std::string name = "mesh_" + std::to_string(mesh_index) + "_skin_" + std::to_string(skin_index);

		if (auto* existing = doCast<Mesh>(getChild(name)))
			return existing;

		auto* mesh = new Mesh(name, "//meworla/mesh");
		mesh->setup(m_app, {}, {});

		if (!addChild(mesh)) {
			delete mesh;
			return nullptr;
		}

		if (!load(mesh, mesh_index, skin_index)) {
			mesh->destroy();
			return nullptr;
		}

		mesh->ready();
		return mesh;
	}

	void GltfDocument::CreateSkeleton(int idx, Skeleton* skeleton) {

		tinygltf::Skin& skin = m_model.skins[idx];
		bool handedness_transform = false;
		m_nodeIndexToBoneIndex.clear();

		// Extract the inverse bind matrices
		math::Matrix4 handedness_conversion(
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, -1, 0, 0,
			0, 0, 0, 1
		);
		std::vector<math::Matrix4> inverseBindMatrices;
		if (skin.inverseBindMatrices != -1) {

			const tinygltf::Accessor& accessor = m_model.accessors[skin.inverseBindMatrices];
			if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || accessor.type != TINYGLTF_TYPE_MAT4) {
				LOG(LOG_ERROR) << "Error: Inverse bind matrices are not in the expected format!";
				return;
			}
			const tinygltf::BufferView& bufferView = m_model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = m_model.buffers[bufferView.buffer];
			const float* data = reinterpret_cast<const float*>(buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);
			size_t stride = (bufferView.byteStride > 0) ? bufferView.byteStride : sizeof(float) * 16;

			size_t matrixCount = accessor.count;
			inverseBindMatrices.resize(matrixCount);
			for (int i = 0; i < matrixCount; ++i) {
				inverseBindMatrices[i] = math::Matrix4(data + i * stride / sizeof(float)); // Assumes column-major format
				if (handedness_transform)
					inverseBindMatrices[i] = handedness_conversion * inverseBindMatrices[i];
			}
		}

		// Create bones
		for (int i = 0; i < skin.joints.size(); ++i) {
			int nodeIndex = skin.joints[i];
			tinygltf::Node& node = m_model.nodes[nodeIndex];
			math::Matrix4 invBindMatrix = (i < inverseBindMatrices.size()) ? inverseBindMatrices[i] : math::Matrix4();

			Bone bone(i, -1, invBindMatrix);
			bone.name = node.name;

			// initialize local_transform
			math::Matrix4 localTransform;

			if (!node.matrix.empty()) {
				// If the node has a matrix, use it directly (GLTF stores this in column-major order)
				localTransform = math::Matrix4(node.matrix.data());
			}
			else {
				// Otherwise, construct the matrix from translation, rotation, and scale
				math::Vector3 translation;
				math::Quaternion rotation;
				math::Vector3 scale;
				if (node.translation.size() < 3)
					translation = math::Vector3(0.0f, 0.0f, 0.0f);
				else
					translation = math::Vector3((float)node.translation[0], (float)node.translation[1], (float)node.translation[2]);
				if (node.rotation.size() < 4)
					rotation = math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
				else {
					// attention: gltf defines the rotation as [x,y,z,w]
					rotation = math::Quaternion((float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2]);
					rotation.normalize();
				}
				if (node.scale.size() < 3)
					scale = math::Vector3(1.0f, 1.0f, 1.0f);
				else
					scale = math::Vector3((float)node.scale[0], (float)node.scale[1], (float)node.scale[2]);

				localTransform = math::Matrix4::compose(translation, rotation, scale);
			}
			if (!handedness_transform)
				bone.local_transform = localTransform;
			else
				bone.local_transform = handedness_conversion * localTransform;

			//  add the bone
			skeleton->addBone(bone);
			m_nodeIndexToBoneIndex[nodeIndex] = i;

#ifdef DEBUG_GLTF
			if (i < 3) {
				LOG(LOG_DEBUG) << "Node gltf-data for bone " << i;
				if (node.translation.size() >= 3)
					LOG(LOG_DEBUG) << "Translation: " << node.translation[0] << ", " << node.translation[1] << ", " << node.translation[2];
				if (node.rotation.size() >= 4) {
					LOG(LOG_DEBUG) << "Rotation: " << node.rotation[3] << ", " << node.rotation[0] << ", " << node.rotation[1] << ", " << node.rotation[2];
					Quaternion rotation = Quaternion((float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[2], (float)node.rotation[2]);
					rotation.normalize();
					math::Matrix3 rotationMatrix = rotation.RotationMatrix();
				}
				if (node.scale.size() >= 3)
					LOG(LOG_DEBUG) << "Scale: " << node.scale[0] << ", " << node.scale[1] << ", " << node.scale[2];
			}
#endif
		}

		// Establish the parent-child relationships within the bones
		for (size_t i = 0; i < skin.joints.size(); ++i) {
			int nodeIndex = skin.joints[i];
			tinygltf::Node& node = m_model.nodes[nodeIndex];

			// Determine the parent relationship only if the node is in the map
			for (int childIndex : node.children) {
				if (m_nodeIndexToBoneIndex.find(childIndex) != m_nodeIndexToBoneIndex.end()) {
					int childBoneIndex = m_nodeIndexToBoneIndex[childIndex];
					int parentBoneIndex = m_nodeIndexToBoneIndex[nodeIndex];

					skeleton->getBones()[childBoneIndex].parent_index = parentBoneIndex;
					skeleton->getBones()[parentBoneIndex].addChild(childBoneIndex);
				}
			}
		}
	}

	void GltfDocument::CreateAnimations(rhea::Mesh* mesh)
	{
		std::vector<Animation*> animations;

		for (const tinygltf::Animation& anim : m_model.animations) {
			Animation* animation = new Animation(anim.name, "//meworla/animation");
			std::unordered_map<int, Animation::AnimationChannel> anim_channels;

			for (const tinygltf::AnimationChannel& channel : anim.channels) {
				const tinygltf::AnimationSampler& sampler = anim.samplers[channel.sampler];

				int target_node = channel.target_node;
				int target_bone = -1;
				// Check if targetNode corresponds to a bone using the mapping
				if (m_nodeIndexToBoneIndex.find(target_node) != m_nodeIndexToBoneIndex.end()) {
					//animChannel.targetNode = m_nodeIndexToBoneIndex[target_node];
					target_bone = m_nodeIndexToBoneIndex[target_node];
				}
				if (target_bone < -1)
					continue;
				if (anim_channels.find(target_bone) == anim_channels.end()) {
					// Ensure a channel exists for this bone
					anim_channels[target_bone] = Animation::AnimationChannel();
					anim_channels[target_bone].targetNode = target_bone;
				}
				Animation::AnimationChannel* animChannel = &anim_channels[target_bone];

				// Determine the property being animated
				if (channel.target_path == "translation" || channel.target_path == "rotation" || channel.target_path == "scale") {
					// Parse keyframe times
					const tinygltf::Accessor& inputAccessor = m_model.accessors[sampler.input];
					const tinygltf::BufferView& inputBufferView = m_model.bufferViews[inputAccessor.bufferView];
					const tinygltf::Buffer& inputBuffer = m_model.buffers[inputBufferView.buffer];
					const float* inputTimes = reinterpret_cast<const float*>(
						&inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset]
						);

					// Parse keyframe values
					const tinygltf::Accessor& outputAccessor = m_model.accessors[sampler.output];
					const tinygltf::BufferView& outputBufferView = m_model.bufferViews[outputAccessor.bufferView];
					const tinygltf::Buffer& outputBuffer = m_model.buffers[outputBufferView.buffer];
					const float* outputValues = reinterpret_cast<const float*>(
						&outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset]
						);

					for (size_t i = 0; i < inputAccessor.count; ++i) {
						float time = inputTimes[i];

						// store latest time as duration
						if (time > animation->duration)
							animation->duration = time;

						// Set the appropriate property (translation, rotation, or scale)
						if (channel.target_path == "translation") {
							Animation::KeyframePosition key_position;
							key_position.time = time;
							key_position.translation = math::Vector3(outputValues[i * 3 + 0], outputValues[i * 3 + 1], outputValues[i * 3 + 2]);
							animChannel->keyframe_positions.push_back(key_position);
						}
						else if (channel.target_path == "rotation") {
							Animation::KeyframeRotation key_rotation;
							key_rotation.time = time;
							key_rotation.rotation = math::Quaternion(
								outputValues[i * 4 + 3], // w
								outputValues[i * 4 + 0], // x
								outputValues[i * 4 + 1], // y
								outputValues[i * 4 + 2]  // z
							);
							animChannel->keyframe_rotations.push_back(key_rotation);
						}
						else if (channel.target_path == "scale") {
							Animation::KeyframeScale key_scale;
							key_scale.time = time;
							key_scale.scale = math::Vector3(outputValues[i * 3 + 0], outputValues[i * 3 + 1], outputValues[i * 3 + 2]);
							animChannel->keyframe_scales.push_back(key_scale);
						}
					}
				}

			}

			// Add the channels to the animation
			for (auto it = anim_channels.begin(); it != anim_channels.end(); ++it) {
				animation->channels.push_back(it->second);
			}

			mesh->addChild(animation);
		}
	}

	void GltfDocument::ready()
	{
		if (m_instance_path != "") {
			m_instance = doCast<GltfDocument>(m_app->getUnitTree()->find(m_instance_path, m_attributes["basepath"], false));
			if (!m_instance)
				LOG(LOG_ERROR) << "Could not find Gltf scene at " << m_instance_path << " (basepath=" << m_attributes["basepath"] << ")";
			else {
				// copy the children
				m_instance->Instantiate(this);
			}
		}
	}

	void GltfDocument::ensureReady(InstanceHandler handler)
	{
		prepare([this, handler](ErrorReport error) {
			if (error.isError()) {
				handler(error, nullptr);
				return;
			}

			if (!m_definition_created) {
				m_definition_created = true;

				auto* definition = getComponent<SceneDefinitionComponent>();
				if (!definition) {
					handler(Error(Error::Code::INVALID_DATA, "SceneDefinitionComponent not found", getPathName()), nullptr);
					return;
				}
				if (!buildSceneDefinition(definition->scene_def)) {
					handler(Error(Error::Code::INVALID_DATA, "Failed to build glTF scene definition", getPathName()), nullptr);
					return;
				}
			}
			handler(Error(), this);
			});
	}

	void GltfDocument::prepare(PrepareHandler handler)
	{
		// check if we have at least one scene
		if (!m_model.scenes.size())
		{
			if (handler)
				handler(Error(Error::Code::INVALID_DATA, "No scenes in glTF model", getPathName()));
			return;
		}

		if (!m_static_materials.created) {
			if (!createMaterials(false)) {
				if (handler)
					handler(Error(Error::Code::INVALID_DATA, "Failed to create materials", getPathName()));
				return;
			}
			m_static_materials.created = true;
		}
		if (!m_skinned_materials.created) {
			if (!createMaterials(true)) {
				if (handler)
					handler(Error(Error::Code::INVALID_DATA, "Failed to create skinned materials", getPathName()));
				return;
			}
			m_skinned_materials.created = true;
		}

		m_prepared = true;
		if (handler)
			handler(Error());
	}

	void GltfDocument::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "path", &GltfDocument::m_gltf_ref, "//meworla/gltf+json", ResourceUsage::Load);
		registerResourceProperty(type, "shader", &GltfDocument::m_shader_ref, "//meworla/shader", ResourceUsage::Load);
	}

	void GltfDocument::Instantiate(Unit* owner)
	{
		for (auto it = m_ordered_children.begin(); it != m_ordered_children.end(); it++)
		{
			Component* child = (*it)->Clone();
			owner->addChild(child);
		}
	}

	bool GltfDocument::importMesh(int mesh_index, int skin_index, Mesh* destination)
	{
		if (mesh_index < 0 || static_cast<size_t>(mesh_index) >= m_model.meshes.size())
			return false;

		const bool skinned = skin_index >= 0;
		if (!createMaterials(skinned))
			return false;

		Shader* shader = getShader(skinned);
		destination->getVertexBuffer()->setVertexSize(shader->getVertexSize());

		if (skinned) {
			// add the skeleton
			Skeleton* skel = new Skeleton("skeleton", "//meworla/skeleton");
			if (m_model.skins.size()) {
				CreateSkeleton(skin_index, skel);
				if (!skel->isValid()) {
					LOG(LOG_ERROR) << "Skelton is invalid - expect wrong visuals";
				}
			}
			else {
				Bone rootBone(0, -1, math::Matrix4());
				skel->addBone(rootBone);
			}
			//addComponent(skel);
			destination->addComponent(skel);

			// add the animations
			CreateAnimations(destination);
		}
		// prepare vertex buffer
		destination->getVertexBuffer()->setVertexSize(shader->getVertexSize());

		const tinygltf::Mesh& mesh = m_model.meshes[mesh_index];		
		for (size_t i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& primitive = mesh.primitives[i];
			if (primitive.indices < 0) break;

			VertexBuffer* vertices = destination->getVertexBuffer();

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

			// Check if the primitive has skinning data
			const tinygltf::Accessor* jointAccessor = nullptr;
			const tinygltf::Accessor* weightAccessor = nullptr;

			auto joints = primitive.attributes.find("JOINTS_0");
			if (joints != primitive.attributes.end()) {
				jointAccessor = &m_model.accessors[joints->second];
			}

			auto weights = primitive.attributes.find("WEIGHTS_0");
			if (weights != primitive.attributes.end()) {
				weightAccessor = &m_model.accessors[weights->second];
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

				if (!skinned) {
					VertexBufferStandard::VertexStandard v;
					v.position = vertex.position;
					v.normal = vertex.normal;
					v.texcoord = vertex.uv;
					v.tangent = { 0.0f, 0.0f, 0.0f };
					//vertices->push_back((float*)&v);
					((VertexBufferStandard*)vertices)->push_back(v);
				}
				else {
					VertexBufferSkeletal::VertexSkeletal v;
					v.position = vertex.position;
					v.normal = vertex.normal;
					v.texcoord = vertex.uv;
					v.tangent = { 0.0f, 0.0f, 0.0f };
					for (int b = 0; b < 4; ++b) {
						v.jointIndices[b] = 0; // Default to first joint
						v.weights[b] = (b == 0) ? 1.0f : 0.0f; // Full influence from the first joint
					}

					// If skinning data is available, set it
					if (jointAccessor && weightAccessor) {
						// safety checks
						if (jointAccessor->count < 4 || weightAccessor->count < 4) {
							LOG(LOG_ERROR) << "Error: GLTF joint/weight data does not provide 4 influences per vertex.";
						}
						if (jointAccessor->type != TINYGLTF_TYPE_VEC4) {
							LOG(LOG_ERROR) << "Error: Joint indices are not in the expected format.";
						}
						if (weightAccessor->componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || weightAccessor->type != TINYGLTF_TYPE_VEC4) {
							LOG(LOG_ERROR) << "Error: Weights are not in the expected format.";
						}

						// handle jointIndices

						const tinygltf::BufferView& jointView = m_model.bufferViews[jointAccessor->bufferView];
						const tinygltf::Buffer& jointBuffer = m_model.buffers[jointView.buffer];

						if (jointAccessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
							// Joint data is stored as uint8_t (unsigned char)
							const uint8_t* jointData = reinterpret_cast<const uint8_t*>(&jointBuffer.data[jointView.byteOffset + jointAccessor->byteOffset]);
							for (int b = 0; b < 4; ++b) {
								v.jointIndices[b] = jointData[4 * j + b]; // Read as uint8_t
								if (v.jointIndices[b] >= MAX_BONES) {
									v.jointIndices[b] = 0; // Fallback to the first joint
									LOG(LOG_ERROR) << "Invalid bone matrix (" << jointData[4 * j + b] << ") in GLTF";
								}
							}
						}
						else if (jointAccessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
							// Joint data is stored as uint16_t (unsigned short)								
							const uint16_t* jointData = reinterpret_cast<const uint16_t*>(&jointBuffer.data[jointView.byteOffset + jointAccessor->byteOffset]);
							for (int b = 0; b < 4; ++b) {
								v.jointIndices[b] = jointData[4 * j + b]; // Read as uint16_t
								if (v.jointIndices[b] >= MAX_BONES) {
									v.jointIndices[b] = 0; // Fallback to the first joint
									LOG(LOG_ERROR) << "Invalid bone matrix (" << jointData[4 * j + b] << ") in GLTF";
								}
							}
						}
						else {
							LOG(LOG_ERROR) << "Error: Unsupported jointAccessor->componentType: " << jointAccessor->componentType;
						}

						// handle weights							

						const tinygltf::BufferView& weightView = m_model.bufferViews[weightAccessor->bufferView];
						const tinygltf::Buffer& weightBuffer = m_model.buffers[weightView.buffer];
						const float* weightData = reinterpret_cast<const float*>(&weightBuffer.data[weightView.byteOffset + weightAccessor->byteOffset]);

						float total_weight = 0.0f;
						for (int b = 0; b < 4; ++b) {
							v.weights[b] = weightData[4 * j + b]; // Assuming weights correspond to joints
							total_weight += v.weights[b];
						}
						// normalize weight if necessary
						if (total_weight > 0.0f && total_weight != 1.0f) {
							for (int b = 0; b < 4; ++b) {
								v.weights[b] /= total_weight;
							}
						}
					}
					else
						LOG(LOG_ERROR) << "jointAccessor or weightAccessor is invalid in gltf file";
					((VertexBufferSkeletal*)vertices)->push_back(v);
				}
			}
			if (skinned) {
				VertexBufferSkeletal* v_data = (VertexBufferSkeletal*)vertices;
				for (int k = 0; k < 10 /*vertices->size()*/; k++) {
					VertexBufferSkeletal::VertexSkeletal* v = v_data->get(k);
#ifdef GLTF_DEBUG
					LOG(LOG_DEBUG) << "Vertex " << k;
					for (int b = 0; b < 4; ++b) {
						LOG(LOG_ERROR) << "Vertex " << k << ", Joint " << b
							<< ": Index = " << v->jointIndices[b]
							<< ", Weight = " << v->weights[b];
					}
#endif // GLTF_DEBUG
				}
			}
			
			const auto& material_set = skinned
				? m_skinned_materials
				: m_static_materials;

			Material* mat = material_set.fallback;

			// indices
			if (primitive.indices >= 0) {
				const tinygltf::Accessor& index_accessor = m_model.accessors[primitive.indices];
				
				if (primitive.material >= 0 &&
					static_cast<size_t>(primitive.material) < material_set.materials.size())
					mat = material_set.materials[primitive.material];
				if (!mat && material_set.materials.size() > 0)
					// still no material - try index 0 for default material
					mat = material_set.materials[0];				

				auto indices = ExtractIndices(index_accessor);
				// Umwandlung von std::vector<unsigned long> zu std::vector<uint32_t>
				std::vector<uint32_t> indices_uint32(indices.begin(), indices.end());
				destination->addSubmesh(mat, indices_uint32);
			}
		}
		return true;
	}

	std::vector<unsigned long> GltfDocument::ExtractIndices(const tinygltf::Accessor& accessor)
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
			LOG(LOG_ERROR) << "GltfDocument::ExtractIndices: Unsupported index component type.";
		}

		return indices;
	}

	void GltfDocument::HandleNode(tinygltf::Node& node, Mesh* destination)
	{
#ifdef GLTF_DEBUG
		LOG(LOG_DEBUG) << "Loading node: " << node.name;
#endif // GLTF_DEBUG

		// if we have a mesh add it as submesh
		if (node.mesh >= 0) {
			importMesh(node.mesh, node.skin, destination);
		}

		// Handle child nodes.
		for (size_t i = 0; i < node.children.size(); i++) {
			HandleNode(m_model.nodes[node.children[i]], destination);
		}
	}

	Shader* GltfDocument::getShader(bool skinned)
	{
		if (!m_shader_ref.empty())
			return m_app->getUnitTree()->resolve(m_shader_ref);

		const std::string shader_id = skinned
			? "//rhea/shaders/skeletal"
			: "//rhea/shaders/standard";

		return m_app->getUnitTree()->resolve(Ref<Shader>(shader_id));
	}

	bool GltfDocument::createMaterials(bool skinned)
	{
		auto& material_set = skinned
			? m_skinned_materials
			: m_static_materials;

		if (material_set.created)
			return true;

		// get the default shader
		Shader* shader = getShader(skinned);
		if (!shader) {
			LOG(LOG_ERROR) << "Could not find shader resource: " << m_shader_ref.id();
			return false;
		}
		const int diffuse_id = shader->getInputSlotId("DIFFUSE");
		const int ambient_id = shader->getInputSlotId("AMBIENT");
		const int specular_id = shader->getInputSlotId("SPECULAR");
		const int emissive_id = shader->getInputSlotId("EMISSIVE");
		const int shininess_id = shader->getInputSlotId("SHININESS");

		std::string suffix = skinned ? "_skinned" : "_static";

		if (!material_set.fallback) {
			Material* fallback = new Material("fallback_material_" + suffix);
			// create a fallback material for meshes that have no material assigned
			fallback->setShader(shader);
			fallback->setValue("shader", shader->getPathName());

			if (diffuse_id >= 0)
				fallback->setColor(diffuse_id, math::Color(1, 1, 1, 1));

			if (ambient_id >= 0)
				fallback->setColor(ambient_id, math::Color(0, 0, 0, 1));

			if (specular_id >= 0)
				fallback->setColor(specular_id, math::Color(0, 0, 0, 1));

			if (emissive_id >= 0)
				fallback->setColor(emissive_id, math::Color(0, 0, 0, 1));

			if (shininess_id >= 0)
				fallback->setFloat(shininess_id, 1.0f);
			addChild(fallback);
			material_set.fallback = fallback;
		}

		// prepare materials

		int basecolor_id = shader->getTextureSlotId("BASECOLOR");
		int normal_id = shader->getTextureSlotId("NORMAL");

		// create the materials from gltf file
		for (const auto& material : m_model.materials) {
			Material* mat = new Material(material.name + suffix);
			// set shader
			mat->setShader(shader);
			mat->setValue("shader", shader->getPathName());

			// set material values

			// traditional shader - fill as good as possible
			// pbr to traditional ambient - not implemented
			//mat->setColor(ambient_id, math::Vector3(material.ambient[0], material.ambient[1], material.ambient[2]));
			// pbr to traditional specular - not implemented
			//mat->setColor(specular_id, math::Vector3(material.specular[0], material.specular[1], material.specular[2]));
			math::Color diffuse(
				(float)material.pbrMetallicRoughness.baseColorFactor[0],
				(float)material.pbrMetallicRoughness.baseColorFactor[1],
				(float)material.pbrMetallicRoughness.baseColorFactor[2]
			);
			mat->setColor(diffuse_id, diffuse);
			math::Color emmissive(
				(float)material.emissiveFactor[0],
				(float)material.emissiveFactor[1],
				(float)material.emissiveFactor[2]
			);
			mat->setColor(emissive_id, emmissive);

			// file baseColor texture
			if (material.pbrMetallicRoughness.baseColorTexture.index >= 0) {
				int texture_index = material.pbrMetallicRoughness.baseColorTexture.index;
				if (texture_index > -1) {
					const int image_index = m_model.textures[texture_index].source;
					auto tex = m_images.find(image_index);
					if (tex != m_images.end()) {
						mat->setTexture(basecolor_id, tex->second);
					}
				}
			}
			// fill normal maps (if any)
			int normal_index = material.normalTexture.index;
			if (normal_index > -1) {
				const int image_index = m_model.textures[normal_index].source;
				auto tex = m_images.find(image_index);
				if (tex != m_images.end()) {
					mat->setTexture(normal_id, tex->second);
				}
			}

			int subsuffix = 1;
			while(m_children.find(mat->getPathName()) != m_children.end()) {
				mat->setName(material.name + suffix + "_" + std::to_string(subsuffix));
				++subsuffix;
			}
			addChild(mat);			
			material_set.materials.push_back(mat);
		}
		material_set.created = true;
		return true;
	}
}