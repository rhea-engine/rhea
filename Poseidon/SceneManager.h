#ifndef POSEIDON_SCENEMANAGER_H
#define POSEIDON_SCENEMANAGER_H

#include <vector>

#include "Camera.h"

namespace Hippios
{
	class Renderer;
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

		Node* createNode();
		Node* get_root();

		// set the render system
		void setRenderer(Hippios::Renderer* renderer) { m_renderer = renderer; }

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
		Viewport* getViewport();
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

		struct cbPerObject
		{
			Poseidon::Matrix4 vp;
		};

		cbPerObject cbPerObj;

	};
}
#endif // !POSEIDON_SCENEMANAGER_H
