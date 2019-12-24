#include "Scene.h"

namespace Poseidon
{
	Node* Scene::get_root()
	{
		if (!m_tree) {
			m_tree = createNode();
		}
		return m_tree;
	}

	Node* Scene::createNode()
	{
		Node* node = new Node(1);
		return node;
	}

	bool Scene::update()
	{
		if (m_tree)
			return m_tree->update();
		else
			return false;
	}
}