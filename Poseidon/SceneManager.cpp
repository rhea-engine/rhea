#include "SceneManager.h"
#include "Node.h"
#include "Viewport.h"

namespace Poseidon
{
	Node* SceneManager::get_root()
	{
		if (!m_tree) {
			m_tree = createNode();
		}
		return m_tree;
	}

	Node* SceneManager::createNode()
	{
		Node* node = new Node();
		return node;
	}

	bool SceneManager::update()
	{
		if (m_tree)
			return m_tree->update();
		else
			return false;
	}

	bool SceneManager::render()
	{
		// TODO for now only the root may be a viewport
		return static_cast<Viewport*>(m_tree)->render();
	}

	Viewport* SceneManager::getViewport()
	{
		// return root as viewprot
		return static_cast<Viewport*>(m_tree);
	}
}