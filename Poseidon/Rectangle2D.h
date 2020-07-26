#ifndef POSEIDON_RECTANGLE2D_H
#define POSEIDON_RECTANGLE2D_H

#include "Mesh.h"

namespace Poseidon
{
	/** a specialized mesh which hold a rectangle for 2d drawing
	*/
	class Rectangle2D : public Mesh
	{
	public:
		Rectangle2D() :
			m_x(-1.0f),
			m_y(-1.0f),
			m_w(1.0f),
			m_h(1.0f),
			m_u1(0.0f),
			m_v1(0.0f),
			m_u2(1.0f),
			m_v2(1.0f)
		{}
		~Rectangle2D();

		// set a dimension of the rectangle in uniform coords
		void setDimension(float x, float y, float w, float h);

		// set texture coords
		void setTextureCoords(float u1, float v1, float u2, float v2);

		// create the buffers
		void createBuffers();
		
	protected:
		float m_x;
		float m_y;
		float m_w;
		float m_h;

		float m_u1;
		float m_v1;
		float m_u2;
		float m_v2;
	};
}

#endif // !POSEIDON_RECTANGLE2D_H
