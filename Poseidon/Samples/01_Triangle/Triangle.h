#include "Rhea/Application.h"
#include "Poseidon/FrameRenderer.h"
#include "Hippios/VertexBuffer.h"

class TriangleApp : public rhea::Application
{
protected:
	poseidon::FrameRenderer* m_scene = 0;

	hippios::vertexVec vertices;
public:
	void setup();
	void initialize();
};