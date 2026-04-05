#include "Rhea/Application.h"
#include "Poseidon/SceneManager.h"
#include "Hippios/VertexBuffer.h"

class TriangleApp : public rhea::Application
{
protected:
	poseidon::SceneManager* m_scene = 0;

	hippios::vertexVec vertices;
public:
	void setup();
	void initialize();
};