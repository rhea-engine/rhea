#include "Terrain.h"
#include "Application.h"
#include "ImageResource.h"
#include "Unit.h"
#include "Mesh.h"
#include "UnitTree.h"
#include <Poseidon/Image.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Shader.h>
#include "Material.h"
#include "TextResource.h"

namespace Rhea
{
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
				Poseidon::Vector3(0, h_top_left, 0),
				Poseidon::Vector3(1, h_top_right, 0),
				Poseidon::Vector3(0, h_bottom_left, 1), 
				xCoord, zCoord);
		}
		else {
			float h_top_right = m_heights[gridX + 1 + gridZ * m_length];
			float h_bottom_right = m_heights[gridX + 1 + (gridZ + 1) * m_length];
			float h_bottom_left = m_heights[gridX + (gridZ + 1) * m_length];

			ret = barryCentric(
				Poseidon::Vector3(1, h_top_right, 0),
				Poseidon::Vector3(1, h_bottom_right, 1),
				Poseidon::Vector3(0, h_bottom_left, 1), 
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

	Poseidon::Vector3 Terrain::calculateNormal(int x, int y)
	{
		float heightL = getHeightAt(x - 1, y);
		float heightR = getHeightAt(x + 1, y);
		float heightD = getHeightAt(x, y - 1);
		float heightU = getHeightAt(x, y + 1);

		Poseidon::Vector3 normal(heightL - heightR, 2.0f, heightD - heightU);
		normal.normalise();

		return normal;
	}

	void Terrain::load(ReadyHandler handler)
	{
		Item::load([&](ReqError err) {
			std::string path = getValue("heightmap");
			m_app->loadResource(path, "//meworla/heightmap", [&](Unit *unit, Orion::ResourceRequest::Error err) {
				if (unit) {
					addChild(unit);

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
						if (static_cast<Resource*>(unit)->getRaw((char**)&buf, &buflen)) {
							m_heightmap->load(buf, buflen);
							m_width = m_heightmap->getWidth();
							m_length = m_heightmap->getHeight();

							// build buffers
							std::vector<Mesh::vertex> vertices;
							std::vector<unsigned long> indices;
							std::vector<Poseidon::Vector3> normals;
							Mesh::vertex vertex;
							m_heights = new float[(long)m_length*(long)m_width];
							
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
						}

						// prepare shaders	
						Material* material = new Material("terrain");
						material->setup(m_app);
						material->setValue("shader", std::string("terrain"));

						m_mesh->setMaterial(material);

						/*
						m_material = m_app->get_core()->getMaterial("terrain");
						if (!m_material) {
							m_material = m_app->get_core()->createMaterial();
							Poseidon::Shader* m_shader = m_app->get_core()->getShader("terrain");
							m_material->setTopology(2);
							if (m_shader->loadFromFile("Data\\Rhea\\shaders\\terrain.hlsl", "VShader", "PShader"))
								exit(-1);
							m_material->setShader(m_shader);
							m_app->get_core()->registerMaterial("terrain", m_material);
						}
						*/
					}
					std::vector<std::string> resnames;
					if (m_values["blendmap"]!=Variant())
						resnames.push_back(m_values["blendmap"]);
					if (m_values["texture1"] != Variant())
						resnames.push_back(m_values["texture1"]);
					if (m_values["texture2"] != Variant())
						resnames.push_back(m_values["texture2"]);
					if (m_values["texture3"] != Variant())
						resnames.push_back(m_values["texture3"]);
					if (m_values["texture4"] != Variant())
						resnames.push_back(m_values["texture4"]);

					int t_size = resnames.size();
					int to_load = t_size;		
					if (!to_load) {
						// missing resources
						m_app->log("Terrain: missing textures");
						return handler(ReqError::CONFIG_ERR);
					}

					// load shader
					m_app->loadResource("//rhea/shaders/terrain.hlsl", "//meworla/shader", [&](Unit* unit, ReqError err) {
						if (unit) {
							m_mesh->getMaterial()->setShader(static_cast<TextResource*>(unit));
						}

						for (int t_idx = 0; t_idx < t_size; t_idx++) {
							m_app->loadResource(resnames[t_idx], "//meworla/texture", [&](Unit* unit, ReqError err) {
								to_load--;
								if (err == ReqError::OK)
									m_mesh->getMaterial()->addTexture(static_cast<ImageResource*>(unit));
								if (to_load <= 0) {
									handler(err);
								}
								});
						}
						});
				}
				else
					handler(err);				
				});
			});
	}

	Unit* Terrain::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Terrain(name, type);
		return ret;
	}

	float Terrain::barryCentric(Poseidon::Vector3 p1, Poseidon::Vector3 p2, Poseidon::Vector3 p3, float x, float z)
	{
		float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
		float l1 = ((p2.z - p3.z) * (x - p3.x) + (p3.x - p2.x) * (z - p3.z)) / det;
		float l2 = ((p3.z - p1.z) * (x - p3.x) + (p1.x - p3.x) * (z - p3.z)) / det;
		float l3 = 1.0f - l1 - l2;
		return l1 * p1.y + l2 * p2.y + l3 * p3.y;
	}
}