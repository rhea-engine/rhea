#ifndef RHEA_SHADER_H
#define RHEA_SHADER_H

#include <Rhea/Unit.h>
#include <Rhea/Resources/TextResource.h>
#include <Rhea/Resources/VertexAttribute.h>

namespace rhea
{
	enum class ShaderBackend
	{
		Unknown,
		D3D11,
		OpenGL
	};

	enum class ShaderStage
	{
		Vertex,
		Fragment
	};

	struct TextureSlotDesc
	{
		std::string propertyName;   // e.g. "BASECOLOR"
		std::string shaderName;     // e.g. "BaseColorTexture"
		int slot;                   // e.g. 0
	};

	class Shader : public Unit
	{
		MECLASS(Shader, Unit)
	public:
		Shader(std::string name, std::string type_name) :
			m_alpha_blending(false),
			m_wvp(false),
			m_num_input_slots(0),
			m_num_input_slots_vs(0),
			m_num_texture_slots(0),
			m_vertex_size(-1),
			m_vertex_elements(-1),
			m_depth_test(true),
			m_depth_write(true),
			Unit(name, "//meworla/shader")
		{}

		enum class InputType {
			kFloat,
			kVector2,
			kVector4,
			kInt
		};

		struct InputSlot {
			size_t slot_id;
			InputType type;
		};

		// setup
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		std::vector<Unit::Export> getReferences();

		// get shader code for specified backend and stage		
		TextResource* getShaderCode(ShaderBackend backend, ShaderStage stage);
		std::string getEntryPoint(ShaderBackend backend, ShaderStage stage) const;
		bool hasShaderCode(ShaderBackend backend, ShaderStage stage) const;

		// stencil/depth testing		
		void setDepthStencilState(std::string test_name) { m_depth_stencil_state = test_name; }
		std::string getDepthStencilState() { return m_depth_stencil_state; }

		// alpha blending
		bool getAlphaBlending() { return m_alpha_blending; }
		void setAlphaBlending(bool value) { m_alpha_blending = value; }

		// depth test
		bool getDepthTest() { return m_depth_test; }
		void setDepthTest(bool value) { m_depth_test = value; }

		// depth write
		bool getDepthWrite() { return m_depth_write; }
		void setDepthWrite(bool value) { m_depth_write = value; }

		// vertex layout
		std::vector<VertexAttribute>& getVertexLayout() { return m_vertex_layout; }

		// get input slot id by name
		int getInputSlotId(std::string name) { if (m_input_slots.find(name) != m_input_slots.end()) return m_input_slots[name]; else return -1; }

		// get max input slot
		int getNumInputSlots() const { return m_num_input_slots; }

		// get id names of input slots
		std::vector<std::string>& getInputSlotNames() { return m_property_names; }

		// get input slot id by name
		InputSlot* getInputSlotVS(std::string name) { if (m_input_slots_vs.find(name) != m_input_slots_vs.end()) return &m_input_slots_vs[name]; else return nullptr; }		

		// get max input slot
		size_t getNumInputSlotsVS() const { return m_num_input_slots_vs; }

		// get id names of input slots
		std::vector<std::string>& getInputSlotNamesVS() { return m_property_names_vs; }

		// get texture input slot id by name
		int getTextureSlotId(std::string name) {
			for (const auto& desc : m_texture_slot_descs) {
				if (desc.propertyName == name)
					return desc.slot;
			}
			return -1;
		}

		// get max texture input slot
		int getNumTextureSlots() const { return m_num_texture_slots; }

		// get vertex size for input layout
		int getVertexSize();

		// get vertex element count for input layout
		int getVertexElements();

		// get texture slot descriptions
		const std::vector<TextureSlotDesc>& getTextureSlots() const { return m_texture_slot_descs; }

	protected:
		std::string m_depth_stencil_state;
		bool m_alpha_blending;
		bool m_depth_test;
		bool m_depth_write;
		bool m_wvp;
		// vertex layout as a vector of element length
		std::vector<VertexAttribute> m_vertex_layout;
		// material input slot ids
		std::map<std::string, int> m_input_slots;
		int m_num_input_slots;
		std::vector<std::string> m_property_names;
		// material input slot for vertex shader
		std::map<std::string, InputSlot> m_input_slots_vs;
		size_t m_num_input_slots_vs;
		std::vector<std::string> m_property_names_vs;

		int m_num_texture_slots;

		// vertex size cache
		int m_vertex_size;
		// vertex element count cache
		int m_vertex_elements;

		// texture slot descriptions
		std::vector<TextureSlotDesc> m_texture_slot_descs;
	};
}

#endif // !RHEA_SHADER_H
