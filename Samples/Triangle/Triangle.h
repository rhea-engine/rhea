#include "Rhea/Application.h"
#include "Poseidon/SceneManager.h"
#include "Hippios/VertexBuffer.h"

class TriangleApp : public Rhea::Application
{
protected:
	Poseidon::SceneManager* m_scene = 0;

	Hippios::vertexVec vertices;
public:
	void setup();
	void initialize();
};