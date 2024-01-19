#include "Poseidon/Material.h"
#include "Poseidon/Shader.h"
#include <Psio/Image/Image.h>
#include <Poseidon/Texture.h>

#include "Resources/TextResource.h"
#include "Event.h"

#include "Fabrics/Core/Material.h"
#include "MaterialManager.h"
#include "./Texture.h"
#include <Psio/Log.h>
#include <Rhea/Fabrics/Core/Shader.h>

namespace Rhea
{
	Element* GraphicsMaterialManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		Material* material_res = doCast<Material>(unit);
		if (!material_res) {
			LOG(LOG_ERROR) << "GraphicsMaterialManager::createElement: no material.";
			return nullptr;
		}

		// create the element
		GraphicsMaterial* gmat = new GraphicsMaterial();		
		gmat->m_material = core->createMaterial();
		std::string mat_name = material_res->getPathName();

		core->registerMaterial(mat_name, gmat->m_material);

		// add the shader
		std::string shader_name = material_res->getValue("shader");
		Poseidon::Shader* shader = core->getShader(shader_name);
		if (!shader) {

			// shader not yet defined in core - search from resources and create the shader
			Shader* shader_res = material_res->getShaderRes();
			TextResource* shader_code;
			if (shader_res) {
				shader_code = shader_res->getShaderCode();
			}
			else
				shader_code = material_res->getShader();

			if (shader_code) {
				char* buf;
				size_t buflen;
				shader_code->getRaw(&buf, &buflen);
				shader = core->createShader(shader_name);
				if (shader->loadFromBuffer((unsigned char*)buf, buflen, "VShader", "PShader"))
					exit(-1);				

				// set transparency
				if (shader_res)
					shader->setAlphaBlending(shader_res->getAlphaBlending());

				// set stencil testing
				if (shader_res) {
					shader->setStencilTest(shader_res->getStencilTest());

					// cull mode
					std::string cull_mode = shader_res->getValue("cull_mode", "back");
					if (cull_mode=="none") {
						shader->setCullMode(false);
					}
				}				

				// compute the shader
				shader->finalize();

				gmat->m_material->setShader(shader);
			}
			else {
				LOG(LOG_ERROR) << "Shader " << shader_name << " not found";
			}
		}
		else {
			gmat->m_material->setShader(shader);
		}

		// now add the textures
		std::vector<ImageResource*> textures = material_res->getTextures();
		for (auto it = textures.begin(); it != textures.end(); ++it) {
			Element* comp = GraphicsTexture::createElement(core, (Unit*)*it);
			if (comp) {
				Poseidon::Image* tex = ((GraphicsTexture*)comp)->get();
				if (tex) {
					gmat->m_material->addTexture((Poseidon::Texture*)tex);
				}
				else {
					DLOG(LOG_DEBUG) << "GraphicsMaterialManager::createElement: empty texture";
				}
			} 
			// TODO what should we do on failure?
		}

		// register to resource
		material_res->addElement("graphics", gmat);

		// register handlers
		unit->registerEventHandler("delete", this, std::bind(&GraphicsMaterialManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));
		//unit->registerEventHandler("update", std::bind(&GraphicsMaterialManager::onUpdate, gmat, std::placeholders::_1, std::placeholders::_2));

		// add to my list
		//m_elements.push_back(gmat);

		return static_cast<Element*>(gmat);
	}
	
	bool GraphicsMaterialManager::onItemDelete(Event* ev, Unit* unit)
	{
		unit->addElement("graphics", nullptr);
		return false;
	}
}