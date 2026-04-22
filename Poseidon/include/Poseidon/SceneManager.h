#ifndef POSEIDON_SCENEMANAGER_H
#define POSEIDON_SCENEMANAGER_H

#include <vector>
#include <memory>

#include "Camera.h"
#include "Window.h"
#include <Math/Vector4.h>

namespace psio
{
	class Engine;
	class Window;
}

namespace hippios
{
	class Renderer;
	class GraphicsBuffer;
	class RenderTarget;
}

namespace poseidon
{
	class Node;
	class Viewport;
	class RenderQueue;
	class Material;
	class RenderWorld;	

	/** The scene that holds and organises the objects in the world
	*/
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		Node* createNode();
		Node* get_root();

		// set the render system
		void setRenderer(hippios::Renderer* renderer);

		// update all elements in the tree
		bool update();

		// render all viewports
		bool render(RenderWorld* world);
		bool renderWindow(Window* window, RenderWorld* world);
		bool renderViewport(Viewport* viewport, hippios::RenderTarget* target, RenderWorld* world);
		bool renderContext(RenderContext* context, hippios::RenderTarget* target, RenderWorld* world);

		// set up the root node
		void setRoot(Node* node) { m_tree = node; }		
		
		// window helper
		Window* getWindow(psio::Window* win);
		void registerWindow(Window* win) { m_windows.push_back(win); }

		Viewport* getViewport() {
			if (!m_windows.empty() && m_windows[0]) {
				return m_windows[0]->getDefaultViewport();
			}
			return nullptr;
		}

	protected:
		Node* m_tree = 0;
		hippios::Renderer* m_renderer;		
		std::vector<Window*> m_windows;
		RenderQueue* m_rend_queue = NULL;
		Material* m_current_material;

		// per object input buffer
		struct cbPerObjectMVP
		{
			math::Matrix3x4  model;
			math::Matrix4 vp;
		};

		cbPerObjectMVP cbPerObjMVP;

		struct cbPerObjectVP
		{
			math::Matrix4 vp;
		};

		cbPerObjectVP cbPerObj;

	public:
		struct cbPerFrame {
			math::Matrix4 v;
			math::Matrix4 p;
			math::Matrix4 vp;
			math::Vector4 cameraPos;			
		};

		struct cbPerObject {
			math::Matrix4 world_matrix;			
			math::Matrix4 inverse;			
		};

		struct cbPerObjectTerrain
		{
			math::Matrix3x4 model;
			math::Matrix4 vp;
		};

		struct Light {
			math::Vector3 direction;
			float Type;
			math::Quaternion position;
			math::Vector3 lightColor;
			float pad2;
			math::Quaternion diffuse;
			math::Quaternion specular;

			float ConeAngle;
			float ConstantAttenuation;
			float LinearAttenuation;
			float QuadraticAttenuation;
		};

#define MAX_LIGHTS 8 

		struct cbLight {		
			Light lights[MAX_LIGHTS];
			math::Quaternion ambient;
			float num_lights = 0.0f;
			float pad2[3] = { 0.0f, 0.0f };
		};

	protected:
		cbPerFrame per_frame_buf;
		cbPerObject per_obj_buf;
		cbPerObjectTerrain cbPerObjTerrain;
		cbLight light_buf;

		std::unique_ptr<hippios::GraphicsBuffer> m_frame_buffer;
		std::unique_ptr<hippios::GraphicsBuffer> m_object_buffer;
		std::unique_ptr<hippios::GraphicsBuffer> m_object_buffer_wvp;
		std::unique_ptr<hippios::GraphicsBuffer> m_light_buffer;
		std::unique_ptr<hippios::GraphicsBuffer> m_bone_buffer;
	};
}
#endif // !POSEIDON_SCENEMANAGER_H
