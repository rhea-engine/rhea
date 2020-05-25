#ifndef POSEIDON_MATERIAL_H
#define POSEIDON_MATERIAL_H

#include "Hippios/Renderer.h"
#include "Hippios/GPUProgram.h"
#include "Hippios/GraphicsBuffer.h"

namespace Poseidon
{
	class Material
	{
	protected:
		Hippios::Renderer* m_renderer;
		int m_type = 0;
	public:		
		Material::Material(Hippios::Renderer* renderer);
		~Material();
		bool loadFromFile(std::string filename, char* vs_entry_point, char* ps_entry_point);
		void initBuffer(size_t size);
		void setType(int type);
		int getType() { return m_type; }

		// set the current material as active material
		void activate();

		Hippios::GPUProgram* getVShader() { return m_vshader; }
		Hippios::GPUProgram* getPShader() { return m_pshader; }
		Hippios::GraphicsBuffer* getCBBuffer() { return m_cb_per_object_buffer; }
	protected:
		Hippios::GPUProgram* m_vshader;
		Hippios::GPUProgram* m_pshader;
		Hippios::GraphicsBuffer* m_cb_per_object_buffer;
	};
}

#endif // !POSEIDON_MATERIAL_H
