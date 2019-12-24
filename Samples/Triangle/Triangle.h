#include "Rhea/Application.h"
#include "Poseidon/Scene.h"
#include "Hippios/VertexBuffer.h"

class TriangleApp : public Rhea::Application
{
protected:
	Poseidon::Scene* m_scene = 0;

	Hippios::vertexVec vertices;
public:
	void setup();
	void initialize();
};