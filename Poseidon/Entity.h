#ifndef POSEIDON_ENTITY_H
#define POSEIDON_ENTITY_H

#include "Animatable.h"
#include "Mesh.h"

namespace Poseidon
{
	class Entity : public Animatable
	{
	protected:
		Mesh* m_mesh = 0;
	public:
		Entity();
		void set_mesh(Mesh* mesh);
		Mesh* get_mesh() { return m_mesh; }		
		bool onUpdate();
		bool render(Viewport* viewport, Camera* camera);

	protected:
		int m_vertex_size = 0;
		Texture* m_texture = 0;

		struct cbPerObjectMVP
		{
			Poseidon::Matrix3x4  model;
			Poseidon::Matrix4 vp;
		};

		cbPerObjectMVP cbPerObjMVP;
	};
}

#endif // !POSEIDON_ENTITY_H
