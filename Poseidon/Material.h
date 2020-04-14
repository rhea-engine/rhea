#ifndef POSEIDON_MATERIAL_H
#define POSEIDON_MATERIAL_H

#include "Hippios/Renderer.h"

namespace Poseidon
{
	class Material
	{
	protected:
		Hippios::Renderer* m_renderer;
		int m_type = 0;
	public:
		Material::Material(Hippios::Renderer* renderer);
		bool loadFromFile(std::string filename, char* shader_type, char* entry_point);
		void initBuffer(size_t size);
		void setType(int type);
	};
}

#endif // !POSEIDON_MATERIAL_H
