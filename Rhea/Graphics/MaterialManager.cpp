#include "Poseidon/Material.h"
#include "Poseidon/Shader.h"
#include <Psio/Image/Image.h>
#include <Poseidon/Texture.h>

#include "TextResource.h"
#include "Event.h"

#include "Fabrics/Core/Material.h"
#include "MaterialManager.h"
#include "./Texture.h"
#include <Psio/Log.h>

namespace Rhea
{
	Element* GraphicsMaterialManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		// create the element
		GraphicsMaterial* item = new GraphicsMaterial();
		Material* material_res = static_cast<Material*>(unit);
		
		item->m_material = core->createMaterial();
		std::string mat_name = material_res->getPathName();

		core->registerMaterial(mat_name, item->m_material);

		// set transparency
		item->m_material->setAlphaBlending(material_res->getAlphaBlending());

		// add the shader
		std::string shader_name = material_res->getValue("shader");
		Poseidon::Shader* shader = core->getShader(shader_name);
		if (!shader) {

			// shader not yet defined in core - search from resources
			TextResource* shader_res = material_res->getShader();
			if (shader_res) {
				char* buf;
				size_t buflen;
				shader_res->getRaw(&buf, &buflen);
				shader = core->createShader(shader_name);
				if (shader->loadFromBuffer((unsigned char*)buf, buflen, "VShader", "PShader"))
					exit(-1);
				item->m_material->setShader(shader);
			}
			else {
				LOG(LOG_ERROR) << "Shader " << shader_name << " not found";
			}
		}
		else {
			item->m_material->setShader(shader);
		}

		// now add the textures
		std::vector<ImageResource*> textures = material_res->getTextures();
		for (auto it = textures.begin(); it != textures.end(); ++it) {
			Element* comp = GraphicsTexture::createElement(core, (Unit*)*it);
			if (comp) {
				Poseidon::Image* tex = ((GraphicsTexture*)comp)->get();
				item->m_material->addTexture((Poseidon::Texture*)tex);
			} 
			// TODO what should we do on failure?
		}

		// register to resource
		material_res->addElement("graphics", item);

		// register handlers
		unit->registerEventHandler("delete", this, std::bind(&GraphicsMaterialManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));
		//unit->registerEventHandler("update", std::bind(&GraphicsMaterialManager::onUpdate, item, std::placeholders::_1, std::placeholders::_2));

		// add to my list
		//m_elements.push_back(item);

		return static_cast<Element*>(item);
	}
	
	bool GraphicsMaterialManager::onItemDelete(Event* ev, Unit* unit)
	{
		Element* el = unit->getElement("graphics");
		if (el)
			delete el;

		return false;
	}
}