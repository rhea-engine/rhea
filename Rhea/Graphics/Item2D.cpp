#include "Item2D.h"
#include "Fabrics/Canvas/Item2D.h"
#include "Poseidon/Entity.h"
#include "Poseidon/SceneManager.h"
#include "Poseidon/Shader.h"
#include "Poseidon/Material.h"
#include "Poseidon/Rectangle2D.h"
#include "Poseidon/RenderContext.h"
#include "TextResource.h"
#include "Application.h"
#include "Texture.h"

namespace Rhea
{
	GraphicsItem2D::~GraphicsItem2D()
	{
		if (m_entity) {
			if (m_render_context)
				m_render_context->removeRenderChild(static_cast<Poseidon::Entity*>(m_entity));
			delete m_entity;
		}
	}

	void GraphicsItem2D::createItem2D(Item2D* item)
	{
		Poseidon::SceneManager* scene_mgr = m_core->get_scene_manager();

		// create the rectangle to draw
		Poseidon::Rectangle2D* rect_mesh = new Poseidon::Rectangle2D();
		rect_mesh->setViewport(m_core
			->get_scene_manager()
			->getViewport());
		rect_mesh->setDimension((float)item->X(), (float)item->Y(), (float)item->W(), (float)item->H());
		rect_mesh->createBuffers();
		m_entity = m_core->createEntity(rect_mesh);
		std::string render_context_name = "HUD";
		m_render_context = scene_mgr->getViewport()->getRenderContext(render_context_name);
		if (!m_render_context) {
			item->getApplication()->log("GraphicsItem2D::createItem2D: render context '" + render_context_name + "' not found");
			return;
		}
		m_render_context->addRenderChild(static_cast<Poseidon::Entity*>(m_entity));

		Poseidon::Position* pos = m_entity->get_position();

		Variant pos_sit = item->getValue("position");
		if (pos_sit != Variant()) {
			float x = pos_sit[0];
			float y = pos_sit[1];
			float z = 0.0f;
			pos->setPosition(x, y, z);
		}
		else
			pos->setPosition(0.0f, 0.0f, 0.0f);

		// material
		string mat_name = item->getMaterial()->getPathName();
		Poseidon::Material* material = m_core->getMaterial(mat_name);
		if (!material)
		{
			material = m_core->createMaterial();
			m_core->registerMaterial(mat_name, material);

			// add the shader
			std::string shader_name = item->getMaterial()->getValue("shader");
			Poseidon::Shader* shader = m_core->getShader(shader_name);
			if (!shader) {

				// shader not yet defined in core - search from resources
				TextResource* shader_res = item->getMaterial()->getShader();
				if (shader_res) {
					char* buf;
					size_t buflen;
					shader_res->getRaw(&buf, &buflen);
					shader = m_core->createShader("terrain");
					if (shader->loadFromBuffer((unsigned char*)buf, buflen, "VShader", "PShader"))
						exit(-1);
					material->setShader(shader);
				}
				else {
					item->getApplication()->log("Shader " + shader_name + "not found");
				}
			}
			else {
				material->setShader(shader);
			}

			// now add the textures
			std::vector<ImageResource*> textures = item->getMaterial()->getTextures();
			for (auto it = textures.begin(); it != textures.end(); ++it) {
				Element* comp = GraphicsTexture::createElement(m_core, (Unit*)*it);
				Poseidon::Image* tex = ((GraphicsTexture*)comp)->get();
				material->addTexture((Poseidon::Texture*)tex);
			}
		}

		static_cast<Poseidon::Entity*>(m_entity)->setMaterial(material);

		item->addElement("graphics", this);
	}

	Element* GraphicsItem2D::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		GraphicsItem2D* item = new GraphicsItem2D(core);
		item->createItem2D(static_cast<Item2D*>(unit));

		return item;
	}
}