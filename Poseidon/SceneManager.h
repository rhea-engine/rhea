#ifndef POSEIDON_SCENEMANAGER_H
#define POSEIDON_SCENEMANAGER_H

#include <vector>

#include "Camera.h"

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
			Poseidon::Matrix3x4  model;
			Poseidon::Matrix4 vp;
		};

		cbPerObjectMVP cbPerObjMVP;

		struct cbPerObjectVP
		{
			Poseidon::Matrix4 vp;
		};

		cbPerObjectVP cbPerObj;

	public:
		struct cbPerFrame {
			Poseidon::Matrix4 v;
			Poseidon::Matrix4 p;
			Poseidon::Matrix4 vp;
		};

		struct cbPerObject {
			Poseidon::Matrix3x4 model;
		};

		struct cbPerObjectTerrain
		{
			Poseidon::Matrix3x4 model;
			Poseidon::Matrix4 vp;
		};

	protected:
		cbPerFrame per_frame_buf;
		cbPerObject per_obj_buf;
		cbPerObjectTerrain cbPerObjTerrain;

		Hippios::GraphicsBuffer* m_frame_buffer;
		Hippios::GraphicsBuffer* m_object_buffer;
	};
}
#endif // !POSEIDON_SCENEMANAGER_H
