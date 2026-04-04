#include "Poseidon/Node.h"

namespace poseidon
{
	Node::~Node()
	{
	}

	Position* Node::get_position()
	{
		if (m_transform)
			return m_transform->get_position();
		else
			return nullptr;
	}

	Position* Node::get_derived_position()
	{
		if (m_transform)
			return m_transform->get_derived_position();
		else
			return nullptr;
	}

	void Node::registerGroup(std::string group_name, Node* node)
	{
		// we store nothing - this has to be done in children

		// not implemented
	}
}