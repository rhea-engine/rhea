#include "Poseidon/Material.h"
#include "Poseidon/Shader.h"
#include <Psio/Image/Image.h>
#include <Poseidon/Texture.h>

#include <Rhea/Resources/TextResource.h>
#include <Rhea/Event.h>

#include <Rhea/Core/Material.h>
#include <Rhea/Graphics/MaterialManager.h>
#include <Rhea/Graphics/Texture.h>
#include <Psio/Log.h>
#include <Rhea/Core/Shader.h>
#include <Rhea/Graphics/GraphicsService.h>

namespace Rhea
{
	static Hippios::InputLayoutFormat ConvertVertexFormat(const Rhea::VertexAttribute& a)
	{
		if (a.element_size == 4 && a.element_count == 2)
			return Hippios::InputLayoutFormat::R32G32_FLOAT;
		if (a.element_size == 4 && a.element_count == 3)
			return Hippios::InputLayoutFormat::R32G32B32_FLOAT;
		if (a.element_size == 4 && a.element_count == 4) {
			if (a.type == Rhea::VertexAttributeType::JOINTS)
				return Hippios::InputLayoutFormat::R32G32B32A32_UINT; // if joints are really uint32
			return Hippios::InputLayoutFormat::R32G32B32A32_FLOAT;
		}
		if (a.element_size == 2 && a.element_count == 4 && a.type == Rhea::VertexAttributeType::JOINTS)
			return Hippios::InputLayoutFormat::R16G16B16A16_UINT;

		return Hippios::InputLayoutFormat::FORMAT_UNKNOWN;
	}

	Poseidon::Shader* GraphicsMaterialManager::EnsurePoseidonShader(Poseidon::Core* core, Rhea::Material* material_res, Rhea::Shader* shader_res)
	{
		if (!material_res || !shader_res)
			return nullptr;
		std::string shader_name = material_res->getValue("shader");
		if (shader_name.empty()) {
			LOG(LOG_ERROR) << "EnsurePoseidonShader: material has empty shader name";
			return nullptr;
		}
		Poseidon::Shader* shader = core->getShader(shader_name);
		if (shader)
			return shader;

		// shader not yet defined in core - search from resources and create the shader		
		auto backend = core->getRendererBackend();

		TextResource* vs_res = nullptr;
		TextResource* fs_res = nullptr;
		ShaderBackend shader_backend = ShaderBackend::Unknown;

		if (backend == "d3d11") {
			vs_res = shader_res->getShaderCode(ShaderBackend::D3D11, ShaderStage::Vertex);
			fs_res = shader_res->getShaderCode(ShaderBackend::D3D11, ShaderStage::Fragment);
			shader_backend = ShaderBackend::D3D11;

		}
		else if (backend == "opengl") {
			vs_res = shader_res->getShaderCode(ShaderBackend::OpenGL, ShaderStage::Vertex);
			fs_res = shader_res->getShaderCode(ShaderBackend::OpenGL, ShaderStage::Fragment);
			shader_backend = ShaderBackend::OpenGL;
		}

		if (!vs_res || !fs_res) {
			LOG(LOG_ERROR) << "GraphicsMaterialManager::EnsurePoseidonShader: Shader code not found for backend \"" << backend << "\"";
			return nullptr;
		}

		char* vs_buf = nullptr;
		size_t vs_len = 0;
		vs_res->getRaw(&vs_buf, &vs_len);

		char* fs_buf = nullptr;
		size_t fs_len = 0;
		fs_res->getRaw(&fs_buf, &fs_len);

		Poseidon::ShaderProgramSourceDesc desc;
		desc.vertexSource = reinterpret_cast<const unsigned char*>(vs_buf);
		desc.vertexSourceLength = vs_len;
		desc.pixelSource = reinterpret_cast<const unsigned char*>(fs_buf);
		desc.pixelSourceLength = fs_len;
		desc.vertexEntry = shader_res->getEntryPoint(shader_backend, ShaderStage::Vertex);
		desc.pixelEntry = shader_res->getEntryPoint(shader_backend, ShaderStage::Fragment);

		shader = core->createShader(shader_name);
		if (!shader->loadFromSourceDesc(desc)) {
			LOG(LOG_ERROR) << "Could not compile shader";
			return nullptr;
		}

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

		// sampler state
		std::string sampler_name = shader_res->getValue("sampler", "linear");
		Hippios::SamplerState* sampler = core->getSamplerState(sampler_name);
		shader->setSamplerState(sampler);

		// input layout
		std::vector<Poseidon::VertexInputAttribute> input_elements;
		const auto& src_layout = shader_res->getVertexLayout();

		for (size_t i = 0; i < src_layout.size(); ++i) {
			const auto& a = src_layout[i];

			Poseidon::VertexInputAttribute e;
			e.semantic = a.name;
			e.semanticIndex = 0;
			e.location = (a.location >= 0) ? static_cast<uint32_t>(a.location) : 0;
			e.format = ConvertVertexFormat(a);
			e.inputSlot = 0;
			e.alignedByteOffset = static_cast<uint32_t>(a.offset);
			e.perInstance = false;
			e.instanceStepRate = 0;

			input_elements.push_back(e);
		}

		shader->setInputLayout(
			input_elements,
			{ Hippios::InputBindingDesc{ 0, (uint32_t)shader_res->getVertexSize(), 0 } }
		);		
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

		return shader;
	}

	bool GraphicsMaterialManager::Ensure(Poseidon::Core* core, Unit* unit)
	{
		return true;
	}
	
	bool GraphicsMaterialManager::FillPoseidonMaterial(Poseidon::Core* core, Material* material_res, Poseidon::Material* out_mat, bool fresh_build)
	{
		if (!core || !material_res || !out_mat) return false; 
		
		// add the shader
		Shader* shader_res = material_res->getShaderRes();
		if (!shader_res) {
			LOG(LOG_ERROR) << "FillPoseidonMaterial: no shader resource";
			return nullptr;
		}
		
		Poseidon::Shader* shader = EnsurePoseidonShader(core, material_res, shader_res);
		if (!shader) return nullptr;
		// Only setShader if actually changed (because setShader reallocates buffers)
		if (out_mat->getShader() != shader) {
			out_mat->setShader(shader);
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
				int slot = out_mat->getShader()->getMatBufLayoutMap(*it);
				if (slot != -1)
					out_mat->SetColor(slot, color);
			}
			int slot_res = shader_res->getInputSlotId("SHININESS");
			if (slot_res >= 0) {
				int slot = out_mat->getShader()->getMatBufLayoutMap("SHININESS");
				if (slot != -1)
					out_mat->SetFloat(slot, material_res->getFloat(slot_res));
			}
		}
		// vs props
		std::vector<std::string> material_names = shader_res->getInputSlotNamesVS();
		for (auto& name : material_names) {
			Shader::InputSlot* slot = shader_res->getInputSlotVS(name);
			// TODO implement other types
			if (slot->type == Shader::InputType::kVector2) {
				Math::Vector2 v2 = material_res->getFloat2VS(slot->slot_id);
				int poseidon_slot = out_mat->getShader()->getMatBufLayoutMapVS(name);
				out_mat->SetFloat2VS(poseidon_slot, v2);
				//out_mat->SetFloat2VS(poseidon_slot, Math::Vector2(2.0f,2.0f));
			}
		}

		// ---- textures ----
		// IMPORTANT: Poseidon::Material only supports addTexture(), no clear.
		// So ONLY add textures when this Poseidon::Material is freshly built.
		if (fresh_build) {
			for (auto* img : material_res->getTextures()) {
				if (!img) continue;
				Poseidon::Texture* t = m_service->textureCache().getPoseidonTexture(core, img);
				if (t)
					out_mat->addTexture(t);
				else
					DLOG(LOG_DEBUG) << "GraphicsMaterialManager::FillPoseidonMaterial: empty texture";
			}
		}

		// register handlers
		//unit->registerEventHandler("delete", this, std::bind(&GraphicsMaterialManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));
		//unit->registerEventHandler("update", std::bind(&GraphicsMaterialManager::onUpdate, gmat, std::placeholders::_1, std::placeholders::_2));

		return true;
	}
	
	bool GraphicsMaterialManager::onItemDelete(Event* ev, Unit* unit)
	{		
		return false;
	}
}