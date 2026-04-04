#include "Rhea/Application.h"
#include "Poseidon/Viewport.h"
#include "Hippios/VertexBuffer.h"

class TriangleTextureApp : public rhea::Application
{
protected:
	Poseidon::SceneManager* m_scene = 0;

	Hippios::vertexUVVec vertices;
public:
	void setup();
	void initialize();
};