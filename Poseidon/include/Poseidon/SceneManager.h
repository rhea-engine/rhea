#ifndef POSEIDON_SCENEMANAGER_H
#define POSEIDON_SCENEMANAGER_H

#include <vector>

#include "Camera.h"
#include <Components/Math/Vector4.h>

namespace Hippios
{
	class Renderer;
	class GraphicsBuffer;
}

namespace Poseidon
{
	class Node;
	class Viewport;
	class RenderQueue;
	class Material;

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
		void setRenderer(Hippios::Renderer* renderer);

		// update all elements in the tree
		bool update();

		// render all viewports
		bool render();

		// set up the root node
		void setRoot(Node* node) { m_tree = node; }
		
		// Viewports
		void registerViewport(Viewport* viewport);
		void deregisterViewport(Viewport* viewport);

		// helper 
		Viewport* getViewport(std::string name = "root");
	protected:
		Node* m_tree = 0;
		Hippios::Renderer* m_renderer;
		std::vector<Viewport*> m_viewports;
		RenderQueue* m_rend_queue = NULL;
		Material* m_current_material;

		// per object input buffer
		struct cbPerObjectMVP
		{
			Math::Matrix3x4  model;
			Math::Matrix4 vp;
		};

		cbPerObjectMVP cbPerObjMVP;

		struct cbPerObjectVP
		{
			Math::Matrix4 vp;
		};

		cbPerObjectVP cbPerObj;

	public:
		struct cbPerFrame {
			Math::Matrix4 v;
			Math::Matrix4 p;
			Math::Matrix4 vp;
			Math::Vector4 cameraPos;			
		};

		struct cbPerObject {
			Math::Matrix4 world_matrix;			
			Math::Matrix4 inverse;			
		};

		struct cbPerObjectTerrain
		{
			Math::Matrix3x4 model;
			Math::Matrix4 vp;
		};

		struct Light {
			Math::Vector3 direction;
			float Type;
			Math::Quaternion position;
			Math::Vector3 lightColor;
			float pad2;
			Math::Quaternion diffuse;
			Math::Quaternion specular;

			float ConeAngle;
			float ConstantAttenuation;
			float LinearAttenuation;
			float QuadraticAttenuation;
		};

#define MAX_LIGHTS 8 

		struct cbLight {		
			Light lights[MAX_LIGHTS];
			Math::Quaternion ambient;
			float num_lights = 0.0f;
			float pad2[3] = { 0.0f, 0.0f };
		};

	protected:
		cbPerFrame per_frame_buf;
		cbPerObject per_obj_buf;
		cbPerObjectTerrain cbPerObjTerrain;
		cbLight light_buf;

		Hippios::GraphicsBuffer* m_frame_buffer;
		Hippios::GraphicsBuffer* m_object_buffer;
		Hippios::GraphicsBuffer* m_object_buffer_wvp;
		Hippios::GraphicsBuffer* m_light_buffer;
		Hippios::GraphicsBuffer* m_bone_buffer;
	};
}
#endif // !POSEIDON_SCENEMANAGER_H
