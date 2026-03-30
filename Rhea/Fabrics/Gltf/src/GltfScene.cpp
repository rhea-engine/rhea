#include <Rhea/Gltf/GltfScene.h>
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
#undef max
#include <algorithm>

#define MAX_BONES 100

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

		m_has_skeleton = !m_model.skins.empty();

		// get the default shader
		std::string shader_name = "";
		Shader* shader_res = nullptr;
		if (!m_has_skeleton) {
			shader_name = mesh->getValueString("shadername", "//rhea/shaders/standard");
			shader_res = doCast<Shader>(m_app->getUnitTree()->find(shader_name, m_attributes["basepath"], false));
			
		}
		else {
			shader_name = mesh->getValueString("shadername", "//rhea/shaders/skeletal");
			shader_res = doCast<Shader>(m_app->getUnitTree()->find(shader_name, m_attributes["basepath"], false));

			// add the skeleton
			Skeleton* skel = new Skeleton("skeleton", "//meworla/skeleton");
			if (m_model.skins.size()) {
				CreateSkeleton(0, skel);
				if (!skel->isValid()) {
					LOG(LOG_ERROR) << "Skelton is invalid - expect wrong visuals";
				}
			}
			else {
				Bone rootBone(0, -1, Math::Matrix4());
				skel->addBone(rootBone);
			}
			//addComponent(skel);
			mesh->addComponent(skel);

			// add the animations
			CreateAnimations(mesh);

		}
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
				ImageResource* tex = doCast<ImageResource>(m_app->getUnitTree()->find(diffuse_tex[i].toString(), mesh->getBasePath(), false));
				diffuse_maps.push_back(tex);
			}
		}
		// prepare normal maps (from mesh options)
		std::vector<ImageResource*> normal_maps;
		Variant normal_tex = mesh->getValue("normal_tex");
		if (normal_tex.isArray()) {
			for (int i = 0; i < normal_tex.size(); i++) {
				ImageResource* tex = doCast<ImageResource>(m_app->getUnitTree()->find(normal_tex[i].toString(), m_attributes["basepath"], false));
				normal_maps.push_back(tex);
			}
		}
		Variant override_materials = mesh->getValue("material");

		if (override_materials.isArray()) {
			// TODO make overriding of material possible
			for (int i = 0; i < override_materials.size(); i++) {
				Material* mat = doCast<Material>(m_app->getUnitTree()->find(override_materials[i].toString(), mesh->getBasePath(), false));
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
				Math::Vector3 diffuse(
					(float)material.pbrMetallicRoughness.baseColorFactor[0],
					(float)material.pbrMetallicRoughness.baseColorFactor[1],
					(float)material.pbrMetallicRoughness.baseColorFactor[2]
				);
				mat->setColor(diffuse_id, diffuse);
				Math::Vector3 emmissive(
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

			mat->setColor(diffuse_id, Math::Vector3(0.8f, 0.8f, 0.8f));
			mat->setColor(emmission_id, Math::Vector3(0.0f, 0.0f, 0.0f));

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

		return true;
	}

	void GltfScene::CreateSkeleton(int idx, Skeleton* skeleton) {

		tinygltf::Skin& skin = m_model.skins[idx];
		bool handedness_transform = false;

		// Extract the inverse bind matrices
		Math::Matrix4 handedness_conversion(
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, -1, 0, 0,
			0, 0, 0, 1
		);
		std::vector<Math::Matrix4> inverseBindMatrices;
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
				inverseBindMatrices[i] = Math::Matrix4(data + i * stride / sizeof(float)); // Assumes column-major format
				if (handedness_transform)
					inverseBindMatrices[i] = handedness_conversion * inverseBindMatrices[i];
			}
		}

		// Create bones
		for (int i = 0; i < skin.joints.size(); ++i) {
			int nodeIndex = skin.joints[i];
			tinygltf::Node& node = m_model.nodes[nodeIndex];
			Math::Matrix4 invBindMatrix = (i < inverseBindMatrices.size()) ? inverseBindMatrices[i] : Math::Matrix4();

			Bone bone(i, -1, invBindMatrix);
			bone.name = node.name;

			// initialize local_transform
			Math::Matrix4 localTransform;

			if (!node.matrix.empty()) {
				// If the node has a matrix, use it directly (GLTF stores this in column-major order)
				localTransform = Math::Matrix4(node.matrix.data());				
			}
			else {
				// Otherwise, construct the matrix from translation, rotation, and scale
				Math::Vector3 translation;
				Math::Quaternion rotation;
				Math::Vector3 scale;
				if (node.translation.size() < 3)
					translation = Math::Vector3(0.0f, 0.0f, 0.0f);
				else
					translation = Math::Vector3((float)node.translation[0], (float)node.translation[1], (float)node.translation[2]);
				if (node.rotation.size() < 4)
					rotation = Math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
				else {
					// attention: gltf defines the rotation as [x,y,z,w]
					rotation = Math::Quaternion((float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2]);
					rotation.normalize();
				}
				if (node.scale.size() < 3)
					scale = Math::Vector3(1.0f,1.0f,1.0f);
				else
					scale = Math::Vector3((float)node.scale[0], (float)node.scale[1], (float)node.scale[2]);

				localTransform = Math::Matrix4::compose(translation, rotation, scale); 				
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
					Math::Matrix3 rotationMatrix = rotation.RotationMatrix();				
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

	void GltfScene::CreateAnimations(Rhea::Mesh* mesh)
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
							key_position.translation = Math::Vector3(outputValues[i * 3 + 0], outputValues[i * 3 + 1], outputValues[i * 3 + 2]);
							animChannel->keyframe_positions.push_back(key_position);
						}
						else if (channel.target_path == "rotation") {
							Animation::KeyframeRotation key_rotation;
							key_rotation.time = time;
							key_rotation.rotation = Math::Quaternion(
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
							key_scale.scale = Math::Vector3(outputValues[i * 3 + 0], outputValues[i * 3 + 1], outputValues[i * 3 + 2]);
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
			Component* child = (*it)->Clone();
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

					if (!m_has_skeleton) {
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
				if (m_has_skeleton) {
					VertexBufferSkeletal* v_data = (VertexBufferSkeletal*)vertices;
					for (int k = 0; k < 10 /*vertices->size()*/; k++) {
						VertexBufferSkeletal::VertexSkeletal* v = v_data->get(k);
						LOG(LOG_DEBUG) << "Vertex " << k;
						for (int b = 0; b < 4; ++b) {
							LOG(LOG_ERROR) << "Vertex " << k << ", Joint " << b
								<< ": Index = " << v->jointIndices[b]
								<< ", Weight = " << v->weights[b];
						}
					}
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