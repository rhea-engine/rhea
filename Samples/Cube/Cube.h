#include "Rhea/Application.h"
#include "Poseidon/Entity.h"
#include "Hippios/VertexBuffer.h"
#include "Poseidon/FrameRenderer.h"
#include "Rhea/InputMap.h"

namespace poseidon
{
	class Mesh;
}

class CubeApp : public rhea::Application
{
protected:
	poseidon::FrameRenderer* m_scene = 0;

	hippios::vertexUVVec vertices;

	poseidon::Entity* cube1 = 0;
	poseidon::Entity* cube2 = 0;

	poseidon::Vector3 m_position;

	//Global Declarations - Interfaces//
	poseidon::Mesh* mesh;

	hippios::Engine* engine;
	hippios::Renderer* renderer;
	poseidon::Camera* m_camera;

	ID3D11InputLayout* vertLayout;

	LPCTSTR WndClassName = "firstwindow";
	HWND hwnd = NULL;
	HRESULT hr;

	const int Width = 1152;
	const int Height = 864;

	float rot = 0.01f;
	poseidon::Matrix4 pv;

	poseidon::Material* m_material;

	struct cbPerObjectMVP
	{
		poseidon::Matrix3x4  model;
		poseidon::Matrix4 vp;
	};

	cbPerObjectMVP cbPerObjMVP;

	bool onFrameBegin();
	bool onInput(std::shared_ptr<psio::InputEvent> evt);

	psio::KeyInputEvent *e_left;
	psio::KeyInputEvent *e_right;
	psio::KeyInputEvent *e_up;
	psio::KeyInputEvent *e_down;
	psio::KeyInputEvent* e_home;
	psio::KeyInputEvent* e_end;

	rhea::InputMap m_input_map;

	//std::unordered_map<psio::InputEvent, std::vector<std::string>, decltype(psio::ievt_hashing), decltype(psio::ievt_equal_fn)> m_input_map;

public:
	void setup();
	void initialize();

	void UpdateScene();

};