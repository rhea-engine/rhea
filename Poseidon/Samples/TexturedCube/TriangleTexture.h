#include "Rhea/Application.h"
#include "Poseidon/Viewport.h"
#include "Hippios/VertexBuffer.h"

class TriangleTextureApp : public rhea::Application
{
protected:
	poseidon::FrameRenderer* m_scene = 0;

	hippios::vertexUVVec vertices;
public:
	void setup();
	void initialize();
};