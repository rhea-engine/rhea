#include "Rhea/Application.h"
#include "Poseidon/Entity.h"
#include "Hippios/VertexBuffer.h"
#include "Poseidon/SceneManager.h"

namespace Poseidon
{
	class Mesh;
}

class CubeApp : public Rhea::Application
{
protected:
	Poseidon::SceneManager* m_scene = 0;

	Hippios::vertexUVVec vertices;

	Poseidon::Entity* cube1 = 0;
	Poseidon::Entity* cube2 = 0;

	Poseidon::Vector3 m_position;

	//Global Declarations - Interfaces//
	Poseidon::Mesh* mesh;
	//Poseidon::Viewport* viewport;
	Hippios::Engine* engine;
	Hippios::Renderer* renderer;
	Poseidon::Camera* m_camera;

	ID3D11InputLayout* vertLayout;

	LPCTSTR WndClassName = "firstwindow";
	HWND hwnd = NULL;
	HRESULT hr;

	const int Width = 1152;
	const int Height = 864;

	XMMATRIX WVP;
	XMMATRIX VP;
	XMMATRIX cube1World;
	XMMATRIX cube2World;
	XMMATRIX camView;
	XMMATRIX camProjection;

	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;

	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;
	float rot = 0.01f;
	Poseidon::Matrix4 pv;

	struct cbPerObjectMVP
	{
		Poseidon::Matrix3x4  model;
#ifdef USE_P_VP
		Poseidon::Matrix4 vp;
#else
		XMMATRIX  VP;
#endif // USE_P_VP

	};

	cbPerObjectMVP cbPerObjMVP;

public:
	void setup();
	void initialize();

	void UpdateScene();
	int messageloop();
	void CleanUp();
};