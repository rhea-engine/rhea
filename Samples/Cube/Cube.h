#include "Rhea/Application.h"
#include "Poseidon/Entity.h"
#include "Hippios/VertexBuffer.h"
#include "Poseidon/SceneManager.h"
#include "Rhea/InputMap.h"

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

	Hippios::Engine* engine;
	Hippios::Renderer* renderer;
	Poseidon::Camera* m_camera;

	ID3D11InputLayout* vertLayout;

	LPCTSTR WndClassName = "firstwindow";
	HWND hwnd = NULL;
	HRESULT hr;

	const int Width = 1152;
	const int Height = 864;

	float rot = 0.01f;
	Poseidon::Matrix4 pv;

	Poseidon::Material* m_material;

	struct cbPerObjectMVP
	{
		Poseidon::Matrix3x4  model;
		Poseidon::Matrix4 vp;
	};

	cbPerObjectMVP cbPerObjMVP;

	bool onFrameBegin();
	bool onInput(std::shared_ptr<Psio::InputEvent> evt);

	Psio::KeyInputEvent *e_left;
	Psio::KeyInputEvent *e_right;
	Psio::KeyInputEvent *e_up;
	Psio::KeyInputEvent *e_down;
	Psio::KeyInputEvent* e_home;
	Psio::KeyInputEvent* e_end;

	Rhea::InputMap m_input_map;

	//std::unordered_map<Psio::InputEvent, std::vector<std::string>, decltype(Psio::ievt_hashing), decltype(Psio::ievt_equal_fn)> m_input_map;

public:
	void setup();
	void initialize();

	void UpdateScene();

};