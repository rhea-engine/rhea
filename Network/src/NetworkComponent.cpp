#include "Rhea/Network/NetworkComponent.h"
#include <Rhea/Core/Item.h>
#include <Psio/Log.h>

namespace Rhea
{
	void NetworkComponent::onMovementChanged(const Math::Vector3& move)
	{
		if (m_role==NET_ROLE_CLIENT) {
			// send the movement to the network
			DLOG(LOG_DEBUG) << "move:" << move.z;
			if (m_delegate)
				m_delegate->changeMovement(m_NUID, move);
		}
	}
	
	void NetworkComponent::onTurnChanged(float turn)
	{
		if (m_role == NET_ROLE_CLIENT) {
			// send the turning to the network
			DLOG(LOG_DEBUG) << "turn " << turn;
			if (m_delegate)
				m_delegate->changeTurn(m_NUID, turn);
		}
	}
	
	void NetworkComponent::setPosition(Position& position)
	{
		m_item->setPosition(position);
	}
	
	void NetworkComponent::onPositionChanged()
	{
		if (m_role == NET_ROLE_SERVER) {
			m_dirty = true;
		}
	}
}
