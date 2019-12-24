#include "Rhea/Application.h"
#include "Poseidon/Scene.h"
#include "Poseidon/Entity.h"
#include "Hippios/VertexBuffer.h"

class CubeApp : public Rhea::Application
{
protected:
	Poseidon::Scene* m_scene = 0;

	Hippios::vertexUVVec vertices;

	Poseidon::Entity* cube1 = 0;
	Poseidon::Entity* cube2 = 0;

	Poseidon::Vector3 m_position;
public:
	void setup();
	void initialize();

	void UpdateScene();
	void DrawScene();
	int messageloop();
};