#include "Terrain.h"
#include "Terrain.h"
#include "Application.h"
#include "Fabrics/Media/ImageResource.h"
#include "Unit.h"
#include "Fabrics/3D/Mesh.h"
#include "UnitTree.h"
#include <Psio/Image/Image.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Shader.h>
#include "Fabrics/Core/Material.h"
#include "TextResource.h"
#include "ResourceList.h"
#include <Psio/Log.h>

namespace Rhea
{
	void Terrain::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item::setup(app, attributes, values);

		m_resource_list = new ResourceList(app);

		m_resource_list->push("heightmap", m_values["heightmap"], "//meworla/heightmap");
		m_resource_list->push("shader", "//rhea/shaders/terrain.hlsl", "//meworla/shader");

		if (m_values["blendmap"] != Variant())
			m_resource_list->push("blendmap", m_values["blendmap"], "//meworla/texture");
		if (m_values["texture1"] != Variant())
			m_resource_list->push("texture1", m_values["texture1"], "//meworla/texture");
		if (m_values["texture2"] != Variant())
			m_resource_list->push("texture2", m_values["texture2"], "//meworla/texture");
		if (m_values["texture3"] != Variant())
			m_resource_list->push("texture3", m_values["texture3"], "//meworla/texture");
		if (m_values["texture4"] != Variant())
			m_resource_list->push("texture4", m_values["texture4"], "//meworla/texture");
	}

	float Terrain::getHeight(float x, float z)
	{
		float gridSquareSize = 1.0f; 
		
		int gridX = (int)x;
		int gridZ = m_length - (int)z;

		if (gridX<0 || gridX>m_width || gridZ<0 || gridZ>m_length)
			return 0.0f;

		float xCoord = std::fmodf(x, gridSquareSize) / gridSquareSize;
		float zCoord = std::fmodf(z, gridSquareSize) / gridSquareSize;

		float ret;
		if (xCoord <= (1 - zCoord)) {
			float h_top_left = m_heights[gridX + m_length * gridZ];
			float h_top_right = m_heights[gridX + 1 + gridZ * m_length];
			float h_bottom_left = m_heights[gridX + (gridZ + 1) * m_length];

			ret = barryCentric(
				Math::Vector3(0, h_top_left, 0),
				Math::Vector3(1, h_top_right, 0),
				Math::Vector3(0, h_bottom_left, 1), 
				xCoord, zCoord);
		}
		else {
			float h_top_right = m_heights[gridX + 1 + gridZ * m_length];
			float h_bottom_right = m_heights[gridX + 1 + (gridZ + 1) * m_length];
			float h_bottom_left = m_heights[gridX + (gridZ + 1) * m_length];

			ret = barryCentric(
				Math::Vector3(1, h_top_right, 0),
				Math::Vector3(1, h_bottom_right, 1),
				Math::Vector3(0, h_bottom_left, 1), 
				xCoord, zCoord);
		}
		return ret;
	}

	float Terrain::getHeightAt(int x, int y)
	{
		if (x<0 || y<0 || x>=m_width || y>=m_length)
			return 0.0f;

		Poseidon::Image::Color c = m_heightmap->getPixel(x, y);
		// "normalise" to grey
		unsigned char height = (unsigned char)(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
		// scale to max_height
		return (float)height / 256 * m_max_height;
	}

	Unit* Terrain::getHeightmap()
	{
		Unit* map = m_app->getUnitTree()->find(m_values["heightmap"]);
		return map;
	}

	Math::Vector3 Terrain::calculateNormal(int x, int y)
	{
		float heightL = getHeightAt(x - 1, y);
		float heightR = getHeightAt(x + 1, y);
		float heightD = getHeightAt(x, y - 1);
		float heightU = getHeightAt(x, y + 1);

		Math::Vector3 normal(heightL - heightR, 2.0f, heightD - heightU);
		normal.normalise();

		return normal;
	}

	void Terrain::ready()
	{
		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;
		Unit::ready();

		Unit* map = (*m_resource_list)["heightmap"];
		if (map) {
			addChild(map);

			unsigned char* buf;
			size_t buflen;
			bool flipZ = true;

			float max_height = getValue("height");
			if (max_height)
				m_max_height = max_height;

			// if not overwritten by mesh build from heightmap
			if (!m_mesh) {

				std::unordered_map<std::string, std::string> attributes;
				std::unordered_map<std::string, Variant> values;
				m_mesh = static_cast<Mesh*>(m_app->getUnitTree()->createUnit(m_pathname + "_mesh", "//meworla/mesh", attributes, values));

				// build vertex/index buffers from image
				m_heightmap = new Poseidon::Image();
				if (static_cast<Resource*>(map)->getRaw((char**)&buf, &buflen)) {
					m_heightmap->load(buf, buflen);
					m_width = m_heightmap->getWidth();
					m_length = m_heightmap->getHeight();

					// build buffers
					std::vector<Mesh::vertex> vertices;
					std::vector<unsigned long> indices;
					std::vector<Math::Vector3> normals;
					Mesh::vertex vertex;
					m_heights = new float[(long)m_length * (long)m_width];

					for (int h = 0; h < m_length; h++) {
						for (int w = 0; w < m_width; w++) {
							vertex.x = (float)w / ((float)m_width - 1) * m_width;
							vertex.y = (float)getHeightAt(w, h);
							if (flipZ) {
								vertex.z = (float)(m_length - 1 - h) / ((float)m_length - 1) * m_length;
								m_heights[h * m_width + w] = vertex.y;
							}
							else {
								vertex.z = (float)h / ((float)m_length - 1) * m_length;
								m_heights[h * m_width + w] = vertex.y;
							}
							normals.push_back(calculateNormal(w, h));

							vertex.u = (float)w / ((float)m_width - 1);
							vertex.v = (float)h / ((float)m_length - 1);

							vertices.push_back(vertex);
						}
					}

					for (int gz = 0; gz < m_length - 1; gz++) {
						for (int gx = 0; gx < m_width - 1; gx++) {
							int topLeft = (gz * m_length) + gx;
							int topRight = topLeft + 1;
							int bottomLeft = ((gz + 1) * m_length) + gx;
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
					m_mesh->setBuffers(vertices, indices);
					m_render_component->setMesh(m_mesh);
				}

				// prepare shaders	
				Material* material = new Material("terrain");
				material->setup(m_app);
				material->setValue("shader", std::string("terrain"));

				m_mesh->setMaterial(material);				
			}

			Material* material = m_mesh->getMaterial();

			// add shader
			Unit* shader = (*m_resource_list)["shader"];
			if (shader)
				material->setShader(static_cast<TextResource*>(shader));
			else
				LOG(LOG_ERROR) << "Terrain: shader not found";

			// add textures
			Unit* texture;
			if (m_values["blendmap"] != Variant()) {
				texture = (*m_resource_list)["blendmap"];
				material->addTexture(static_cast<ImageResource*>(texture));
			}
			if (m_values["texture1"] != Variant()) {
				texture = (*m_resource_list)["texture1"];
				material->addTexture(static_cast<ImageResource*>(texture));
			}
			if (m_values["texture2"] != Variant()) {
				texture = (*m_resource_list)["texture2"];
				material->addTexture(static_cast<ImageResource*>(texture));
			}
			if (m_values["texture3"] != Variant()) {
				texture = (*m_resource_list)["texture3"];
				material->addTexture(static_cast<ImageResource*>(texture));
			}
			if (m_values["texture4"] != Variant()) {
				texture = (*m_resource_list)["texture4"];
				material->addTexture(static_cast<ImageResource*>(texture));
			}
		}
	}

	Unit* Terrain::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Terrain(name, type);
		return ret;
	}

	float Terrain::barryCentric(Math::Vector3 p1, Math::Vector3 p2, Math::Vector3 p3, float x, float z)
	{
		float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
		float l1 = ((p2.z - p3.z) * (x - p3.x) + (p3.x - p2.x) * (z - p3.z)) / det;
		float l2 = ((p3.z - p1.z) * (x - p3.x) + (p1.x - p3.x) * (z - p3.z)) / det;
		float l3 = 1.0f - l1 - l2;
		return l1 * p1.y + l2 * p2.y + l3 * p3.y;
	}
}