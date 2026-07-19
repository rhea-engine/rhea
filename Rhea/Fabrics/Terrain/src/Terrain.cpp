/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#define NOMINMAX
#include <Rhea/Terrain/Terrain.h>
#include <Rhea/Application.h>
#include <Rhea/Resources/ImageResource.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/UnitTree.h>
#include <Psio/Image/Image.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Core/Shader.h>
#include <Psio/Log.h>
#include <Math/Vector2.h>
#include <Rhea/Components/MeshComponent.h>
#include <cmath>
#include <Rhea/Components/PhysicsDataComponent.h>

namespace rhea
{
	Terrain::~Terrain()
	{
		if (m_heights)
			delete[] m_heights;
	}

	void Terrain::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item::setup(app, attributes, values);

		m_values["shader"] = "//rhea/shaders/terrain";	

		// find or create physics data component

		// find by type from children
		PhysicsDataComponent* pc = nullptr;
		for (auto child : m_children) {
			if (doCast<PhysicsDataComponent>(child.second)) {
				pc = doCast<PhysicsDataComponent>(child.second);
				break;
			}
		}
		if (!pc)
			pc = addComponentUnique<PhysicsDataComponent>("physics_data");
		pc->rigid_body.motion = RigidBody::MotionType::Static;
		pc->rigid_body.kind = "RheaPhysics:HeightMap";
		pc->rigid_body.mass = 0.0f;

		// get size from setup
		if (m_values.find("sizeX") != m_values.end())
			m_sizeX = m_values["sizeX"].toInt();
		if (m_values.find("sizeY") != m_values.end())
			m_sizeY = m_values["sizeY"].toInt();
	}

	std::vector<Unit::ResourceReference> Terrain::getReferences()
	{
		std::vector<ResourceReference> refs = Item::getReferences();
		if (m_values["heightmap"] != Variant())
			refs.push_back({ "heightmap", "//meworla/heightmap", m_values["heightmap"] });
		if (m_values["shader"] != Variant())
			refs.push_back({ "shader", "//meworla/shader", m_values["shader"] });
		if (m_values["blendmap"] != Variant())
			refs.push_back({ "blendmap", "//meworla/texture", m_values["blendmap"] });
		if (m_values["texture1"] != Variant())
			refs.push_back({ "texture1", "//meworla/texture", m_values["texture1"] });
		if (m_values["texture2"] != Variant())
			refs.push_back({ "texture2", "//meworla/texture", m_values["texture2"] });
		if (m_values["texture3"] != Variant())
			refs.push_back({ "texture3", "//meworla/texture", m_values["texture3"] });
		if (m_values["texture4"] != Variant())
			refs.push_back({ "texture4", "//meworla/texture", m_values["texture4"] });
		return refs;
	}

	float Terrain::getHeight(float x, float z)
	{
		float cellSizeX =
			float(m_sizeX) / float(m_width - 1);

		float cellSizeZ =
			float(m_sizeY) / float(m_length - 1);
		
		int gridX = (int)x;
		int gridZ = m_length - (int)z;

		if (gridX<0 || gridX>m_width || gridZ<0 || gridZ>m_length)
			return 0.0f;

		float xCoord = fmodf(x, cellSizeX) / cellSizeX;
		float zCoord = fmodf(z, cellSizeZ) / cellSizeZ;

		float ret;
		if (xCoord <= (1 - zCoord)) {
			float h_top_left = m_heights[gridX + m_length * gridZ];
			float h_top_right = m_heights[gridX + 1 + gridZ * m_length];
			float h_bottom_left = m_heights[gridX + (gridZ + 1) * m_length];

			ret = barryCentric(
				math::Vector3(0, h_top_left, 0),
				math::Vector3(1, h_top_right, 0),
				math::Vector3(0, h_bottom_left, 1), 
				xCoord, zCoord);
		}
		else {
			float h_top_right = m_heights[gridX + 1 + gridZ * m_length];
			float h_bottom_right = m_heights[gridX + 1 + (gridZ + 1) * m_length];
			float h_bottom_left = m_heights[gridX + (gridZ + 1) * m_length];

			ret = barryCentric(
				math::Vector3(1, h_top_right, 0),
				math::Vector3(1, h_bottom_right, 1),
				math::Vector3(0, h_bottom_left, 1), 
				xCoord, zCoord);
		}
		return ret;
	}

	float Terrain::getHeightAt(int x, int y)
	{
		if (x<0 || y<0 || x>=m_width || y>=m_length)
			return 0.0f;

		poseidon::Image::Color c = m_heightmap->getPixel(x, y);
		// "normalize" to grey
		unsigned char height = (unsigned char)(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
		// scale to max_height
		return (float)height / 255 * m_max_height;
	}

	Unit* Terrain::getHeightmap()
	{
		Unit* map = doCast<Unit>(m_app->getUnitTree()->find(m_values["heightmap"].toString()));
		return map;
	}

	math::Vector3 Terrain::calculateNormal(int x, int y)
	{
		int left = std::max(x - 1, 0);
		int right = std::min(x + 1, m_width - 1);
		int lower = std::max(y - 1, 0);
		int upper = std::min(y + 1, m_length - 1);

		float heightL = getHeightAt(left, y);
		float heightR = getHeightAt(right, y);
		float heightD = getHeightAt(x, lower);
		float heightU = getHeightAt(x, upper);

		math::Vector3 normal(heightL - heightR, 2.0f, heightD - heightU);
		normal.normalize();

		return normal;
	}

	void Terrain::ready()
	{
		Item::ready();

		if (m_values.find("heightmap") != m_values.end()) {
			Unit* map = doCast<Unit>(m_app->getUnitTree()->find(m_values["heightmap"].toString(), m_attributes["basepath"], false));
			if (map) {
				
				unsigned char* buf = nullptr;
				size_t buflen;
				bool flipZ = true;

				float max_height = getValue("height");
				if (max_height)
					m_max_height = max_height;

				// make sure visualcomponent is set
				VisualComponent* visual_component = getComponent<VisualComponent>();
				if (!visual_component) {
					visual_component = addComponentUnique<VisualComponent>("visual");
				}

				MeshComponent* mesh_component = getComponent<MeshComponent>();
				if (!mesh_component) {
					mesh_component = addComponentUnique<MeshComponent>("mesh");					
				}

				if (!m_material) {
					m_material = new Material("terrain");
					m_material->setup(m_app, {}, {});
					m_material->setValue("shader", std::string("terrain"));
					addChild(m_material);

					// add shader
					Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(m_values["shader"].toString(), m_attributes["basepath"], false));
					if (shader_res) {
						m_material->setShader(shader_res);
					}
					else {
						LOG(LOG_ERROR) << "Terrain::ready(): Shader '" << (std::string)m_values["shader"] << "' for " << m_pathname << " not found";
						return;
					}
				}

				// if not overwritten by mesh build from heightmap
				if (!mesh_component->getMesh()) {

					// vertex buffers
					std::unordered_map<std::string, VertexBuffer> vertices;
					vertices["POSITION"] = VertexBuffer(3 * sizeof(float));
					vertices["NORMAL"] = VertexBuffer(3 * sizeof(float));
					vertices["TEXCOORD"] = VertexBuffer(2 * sizeof(float));
					// indices
					std::vector<uint32_t> indices;

					struct vertex_norm_uv {
						math::Vector3 position;
						math::Vector3 normal;
						math::Vector2 uv;
					};

					std::unordered_map<std::string, std::string> attributes;
					std::unordered_map<std::string, Variant> values;					
					Mesh* mesh = m_app->getUnitTree()->createUnit<Mesh>(m_pathname + "_mesh", attributes, values);

					mesh->setMaterial(m_material);

					// build vertex/index buffers from image
					m_heightmap = std::make_unique<poseidon::Image>();
					if (static_cast<Resource*>(map)->getRaw((char**)&buf, &buflen)) {
						m_heightmap->load(buf, buflen);
						m_width = m_heightmap->getWidth();
						m_length = m_heightmap->getHeight();
						if (m_sizeX < 0) m_sizeX = m_width;
						if (m_sizeY < 0) m_sizeY = m_length;

						// build buffers
						size_t vert_count = (size_t)(m_width * m_length);
						vertices["POSITION"].reserve(vert_count);
						vertices["NORMAL"].reserve(vert_count);
						vertices["TEXCOORD"].reserve(vert_count);

						vertex_norm_uv vertex;
						m_heights = new float[vert_count];

						for (int h = 0; h < m_length; h++) {
							for (int w = 0; w < m_width; w++) {
								vertex.position.x = (float)w / ((float)m_width - 1) * m_sizeX;
								vertex.position.y = (float)getHeightAt(w, h);
								if (flipZ) {
									vertex.position.z = (float)(m_length - 1 - h) / ((float)m_length - 1) * m_sizeY;
									m_heights[h * m_width + w] = vertex.position.y;
								}
								else {
									vertex.position.z = (float)h / ((float)m_length - 1) * m_sizeY;
									m_heights[h * m_width + w] = vertex.position.y;
								}
								vertex.normal = calculateNormal(w, h);								

								vertex.uv.x = (float)w / ((float)m_width - 1);
								vertex.uv.y = (float)h / ((float)m_length - 1);

								vertices["POSITION"].push_back(&vertex.position.x);								
								vertices["NORMAL"].push_back(&vertex.normal.x);
								vertices["TEXCOORD"].push_back(&vertex.uv.x);
							}
						}

						for (int gz = 0; gz < m_length - 1; gz++) {
							for (int gx = 0; gx < m_width - 1; gx++) {
								int topLeft = (gz * m_width) + gx;
								int topRight = topLeft + 1;
								int bottomLeft = ((gz + 1) * m_width) + gx;
								int bottomRight = bottomLeft + 1;
								if (!flipZ) {
									indices.push_back(topLeft);
									indices.push_back(bottomLeft);
									indices.push_back(topRight);

									indices.push_back(topRight);
									indices.push_back(bottomLeft);
									indices.push_back(bottomRight);
								}
								else {
									indices.push_back(topLeft);
									indices.push_back(topRight);
									indices.push_back(bottomLeft);

									indices.push_back(topRight);
									indices.push_back(bottomRight);
									indices.push_back(bottomLeft);
								}
							}
						}
						mesh->setBuffers(vertices, indices, vert_count);
					}
					mesh_component->setMesh(mesh);
					addChild(mesh);					
				}					

				// add textures
				ImageResource* texture;
				if (m_values["blendmap"] != Variant()) {
					texture = doCast<ImageResource>(m_app->getUnitTree()->find(m_values["blendmap"].toString(), m_attributes["basepath"], false));
					if (texture)
						m_material->setTexture(0, texture);
				}
				if (m_values["texture1"] != Variant()) {
					texture = doCast<ImageResource>(m_app->getUnitTree()->find(m_values["texture1"].toString(), m_attributes["basepath"], false));
					if (texture)
						m_material->setTexture(1, texture);
				}
				if (m_values["texture2"] != Variant()) {
					texture = doCast<ImageResource>(m_app->getUnitTree()->find(m_values["texture2"].toString(), m_attributes["basepath"], false));
					if (texture)
						m_material->setTexture(2, texture);
				}
				if (m_values["texture3"] != Variant()) {
					texture = doCast<ImageResource>(m_app->getUnitTree()->find(m_values["texture3"].toString(), m_attributes["basepath"], false));
					if (texture)
						m_material->setTexture(3, texture);
				}
				if (m_values["texture4"] != Variant()) {
					texture = doCast<ImageResource>(m_app->getUnitTree()->find(m_values["texture4"].toString(), m_attributes["basepath"], false));
					if (texture)
						m_material->setTexture(4, texture);
				}
			}
		}
		else {
			LOG(LOG_ERROR) << "No map defined for terrain";
		}
	}

	float Terrain::barryCentric(math::Vector3 p1, math::Vector3 p2, math::Vector3 p3, float x, float z)
	{
		float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
		float l1 = ((p2.z - p3.z) * (x - p3.x) + (p3.x - p2.x) * (z - p3.z)) / det;
		float l2 = ((p3.z - p1.z) * (x - p3.x) + (p1.x - p3.x) * (z - p3.z)) / det;
		float l3 = 1.0f - l1 - l2;
		return l1 * p1.y + l2 * p2.y + l3 * p3.y;
	}
}