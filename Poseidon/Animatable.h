#ifndef POSEIDON_ANIMATABLE_H
#define POSEIDON_ANIMATABLE_H

namespace Poseidon
{
	class Node;

	/** An object that can be animated ie can be updated per frame or per timeslot and can be attached to a node
	*/
	class Animatable
	{
	protected:
		Node* m_node = 0;		
	public:
		void set_node(Node* node) {
			m_node = node;
		}
		virtual bool onUpdate() = 0;
		Node* get_node() { return m_node; }
	};
}
#endif // !POSEIDON_ANIMATABLE_H
