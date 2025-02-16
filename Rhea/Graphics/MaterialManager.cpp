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
#include "GraphicsComponent.h"

namespace Rhea
{
	GraphicsElement* GraphicsMaterialManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		GraphicsComponent* el = unit->getComponent<GraphicsComponent>();
		if (el)
			return el->getElement();

		Material* material_res = doCast<Material>(unit);
		if (!material_res) {
			LOG(LOG_ERROR) << "GraphicsMaterialManager::createElement: no material.";
			return nullptr;
		}

		// create the element
		GraphicsMaterial* gmat = new GraphicsMaterial();		
		gmat->m_material = core->createMaterial();
		std::string mat_name = material_res->getPathName();

		core->registerMaterial(mat_name, gmat->m_material);   // is this needed?

		// add the shader
		Shader* shader_res = material_res->getShaderRes();
		if (!shader_res) {
			LOG(LOG_ERROR) << "GraphicsMaterialManager::createElement: no shader.";
			return nullptr;
		}
		std::string shader_name = material_res->getValue("shader");
		Poseidon::Shader* shader = core->getShader(shader_name);
		if (!shader) {

			// shader not yet defined in core - search from resources and create the shader			
			TextResource* shader_code;
			shader_code = shader_res->getShaderCode();

			if (!shader_code) {
				LOG(LOG_ERROR) << "Shader code not found";
				return nullptr;
			}

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
				if (cull_mode == "none") {
					shader->setCullMode(false);
				}
			}

			// input layout
			std::string input_layout = shader_res->getValue("input", "");
			if (input_layout == "default") {
				shader->setVertexType(0);
			}
			else if (input_layout == "standard") {
				shader->setVertexType(3);
			}
			else if (input_layout == "short") {
				shader->setVertexType(1);
			}
			else if (input_layout == "color") {
				shader->setVertexType(2);
			}
			else if (input_layout == "skeletal") {
				shader->setVertexType(4);
			}

			// material buffer layout
			std::string light_type = shader_res->getValue("light_type", "none");
			if (light_type == "vertex") {
				// for now only the default layout is supported
				std::vector<Poseidon::Shader::MatBufferLayoutElement> elements;
				elements = {
					{ Poseidon::Shader::MatBufferLayoutElementType::VECTOR4, "DIFFUSE", 0, sizeof(Math::Vector4) },
					{ Poseidon::Shader::MatBufferLayoutElementType::VECTOR4, "AMBIENT", 16, sizeof(Math::Vector4) },
					{ Poseidon::Shader::MatBufferLayoutElementType::VECTOR4, "SPECULAR", 32, sizeof(Math::Vector4) },
					{ Poseidon::Shader::MatBufferLayoutElementType::VECTOR4, "EMISSIVE", 48, sizeof(Math::Vector4) },
					{ Poseidon::Shader::MatBufferLayoutElementType::FLOAT, "SHININESS", 64, sizeof(float) }
				};

				shader->setMatBufLayout(elements);				
			}
			else if (shader_res->getNumInputSlots()) {
				// TODO: really set the elements (not copy from light)
				std::vector<Poseidon::Shader::MatBufferLayoutElement> elements;
				elements = {
					{ Poseidon::Shader::MatBufferLayoutElementType::VECTOR4, "BaseColor", 0, sizeof(Math::Vector4) },
					{ Poseidon::Shader::MatBufferLayoutElementType::VECTOR4, "DIFFUSE", 16, sizeof(Math::Vector4) },
					{ Poseidon::Shader::MatBufferLayoutElementType::VECTOR4, "SPECULAR", 32, sizeof(Math::Vector4) },
					{ Poseidon::Shader::MatBufferLayoutElementType::VECTOR4, "EMISSIVE", 48, sizeof(Math::Vector4) },
					{ Poseidon::Shader::MatBufferLayoutElementType::FLOAT, "SHININESS", 64, sizeof(float) },
					{ Poseidon::Shader::MatBufferLayoutElementType::FLOAT, "NumTextures", 68, sizeof(int) }
				};

				shader->setMatBufLayout(elements);
			}
			// add vertex shader material buffer
			if (shader_res->getNumInputSlotsVS()) {
				std::vector<Poseidon::Shader::MatBufferLayoutElement> elements;
				std::vector<std::string> material_names = shader_res->getInputSlotNamesVS();
				size_t size = 0;
				for (auto& name : material_names) {
					Shader::InputSlot* slot = shader_res->getInputSlotVS(name);
					elements.push_back({ Poseidon::Shader::MatBufferLayoutElementType::VECTOR2, name, size, sizeof(Math::Vector2) });
					size += sizeof(Math::Vector2);
				}
				shader->setMatBufLayoutVS(elements);
			}

			// compute the shader
			shader->finalize();

			gmat->m_material->setShader(shader);
		}
		else {
			gmat->m_material->setShader(shader);
		}

		// now add the material properties
		std::string light_type = shader_res->getValue("light_type", "none");
		std::vector<std::string> props = shader_res->getInputSlotNames();
		if (props.size()) {
			for (auto it = props.begin(); it != props.end(); ++it) {
				Math::Vector4 color;
				int slot_res = shader_res->getInputSlotId(*it);
				if (slot_res < 0)
					continue;
				color = material_res->getColor(slot_res);
				int slot = gmat->m_material->getShader()->getMatBufLayoutMap(*it);
				if (slot != -1)
					gmat->m_material->SetColor(slot, color);
			}
			int slot_res = shader_res->getInputSlotId("SHININESS");
			if (slot_res >= 0) {
				int slot = gmat->m_material->getShader()->getMatBufLayoutMap("SHININESS");
				if (slot != -1)
					gmat->m_material->SetFloat(slot, material_res->getFloat(slot_res));
			}
		}
		// vs props
		std::vector<std::string> material_names = shader_res->getInputSlotNamesVS();
		for (auto& name : material_names) {
			Shader::InputSlot* slot = shader_res->getInputSlotVS(name);
			// TODO implement other types
			if (slot->type == Shader::InputType::kVector2) {
				Math::Vector2 v2 = material_res->getFloat2VS(slot->slot_id);
				int poseidon_slot = gmat->m_material->getShader()->getMatBufLayoutMapVS(name);
				gmat->m_material->SetFloat2VS(poseidon_slot, v2);
				//gmat->m_material->SetFloat2VS(poseidon_slot, Math::Vector2(2.0f,2.0f));
			}
		}

		// now add the textures
		std::vector<ImageResource*> textures = material_res->getTextures();
		for (auto it = textures.begin(); it != textures.end(); ++it) {
			if (!*it)
				continue;
			GraphicsElement* comp = GraphicsTexture::createElement(core, (Unit*)*it);
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
		material_res->addComponentUnique<GraphicsComponent>(gmat);

		// register handlers
		//unit->registerEventHandler("delete", this, std::bind(&GraphicsMaterialManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));
		//unit->registerEventHandler("update", std::bind(&GraphicsMaterialManager::onUpdate, gmat, std::placeholders::_1, std::placeholders::_2));

		// add to my list
		//m_elements.push_back(gmat);

		return gmat;
	}
	
	bool GraphicsMaterialManager::onItemDelete(Event* ev, Unit* unit)
	{		
		return false;
	}
}