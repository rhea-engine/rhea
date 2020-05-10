#include "Poseidon/Mesh.h"
#include "Mesh.h"
#include <Poseidon\Core.h>
#include <Poseidon/SceneManager.h>
#include "Application.h"
#include "Importer.h"
#include "Orion/ResourceStream.h"
#include "UnitTree.h"
#include "TextResource.h"
#include "ImageResource.h"

using Hippios::vertex_uv;
using Poseidon::Viewport;

namespace Rhea
{
	void Mesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		m_app = app;
		m_values = values;
		m_attributes = attributes;
	}

	void Mesh::load(ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			handler(ReqError::OK);

		// load all dependend resources

		std::string mtlbuf;
		std::string objbuf;

		m_app->loadResource(m_values["material"], "//meworla/wavefront-material", [&](Unit* unit, ReqError err) {
			if (unit) {
				mtlbuf = ((TextResource*)unit)->getAsString();
			}
			if (m_values["path"] == "") {
				m_status = Status::CONFIG_ERR;
				return handler(ReqError::CONFIG_ERR);
			}
			m_app->loadResource(m_values["path"], "//meworla/wavefront-object", [&](Unit* unit, ReqError err) {
				if (unit) {
					m_status = Status::LOADED;
					objbuf = ((TextResource*)unit)->getAsString();
					createMesh(objbuf, mtlbuf);
					// load and add textures
					if (m_values["texture"] != "")
					{
						m_app->loadResource(m_values["texture"], "//meworla/texture", [&](Unit* unit, ReqError err) {
							if (unit) {
								Poseidon::Image* tex = ((ImageResource*)unit)->get();
								m_mesh->addTexture((Poseidon::Texture*)tex);
								handler(ReqError::OK);
							}
							else
								handler(err);
							});
					}
					else
						handler(ReqError::OK);
				}
				else {
					m_status = Status::NOT_FOUND;
					handler(err);
				}
				});
			});
	}

	Poseidon::Mesh* Mesh::getMesh()
	{
		if (m_mesh)
			return m_mesh;
		else
			return NULL;
	}

	void Mesh::createMesh(std::string objbuf, std::string mtlbuf)
	{
		Poseidon::Core* core = m_app->get_core();
		Viewport* viewport = m_app->get_core()->get_scene_manager()->getViewport();

		m_mesh = new Poseidon::Mesh();
		m_mesh->setViewport(viewport);

		std::vector<Hippios::vertex_uv> vertices;
		std::vector<unsigned long> v_indices;
		Importer importer;

		importer.load(m_app, objbuf, mtlbuf, vertices, v_indices);
		m_mesh->setVertices(sizeof(Hippios::vertex_uv), vertices.size(), vertices.data());
		m_mesh->setIndices(sizeof(unsigned long), v_indices.size() / 3, v_indices.data());
	}

	Unit* Mesh::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Mesh(name);
		return ret;
	}
}