#ifndef RHEA_NETWORKCOMPONENT_H
#define RHEA_NETWORKCOMPONENT_H

#include <Math/Vector3.h>
#include <Rhea/Core/Position.h>
#include <Rhea/Component.h>
#include <Rhea/Components/MovementListenerComponent.h>
#include "NetworkDelegate.h"

#define NET_ROLE_NONE 0
#define NET_ROLE_SERVER 1
#define NET_ROLE_PROXY 2
#define NET_ROLE_CLIENT 3

namespace Rhea
{
	class Item;

	class NetworkComponent : public MovementListenerComponent
	{
		MECLASS_VIRTUAL(NetworkComponent, MovementListenerComponent)
	public:
		NetworkComponent(Item* item, int nuid, int role) :
			m_NUID(nuid),
			m_item(item),
			m_role(role),
			m_dirty(false),
			m_delegate(nullptr),
			MovementListenerComponent("network", "//meworla/networkcomponent")
		{}

		int NUID() { return m_NUID; }

		// register network handler
		void setDelegate(NetworkDelegate* delegate) { m_delegate = delegate; }

		// handle movement requests
		void onMovementChanged(const Math::Vector3& move) override;
		void onTurnChanged(float turn) override;		

		// handle authoritative position setting

		// set the items position
		void setPosition(Position& position);

		// on Position change (used by server physics)
		void onPositionChange();

		// update role
		void setRole(int role) { m_role = role; }

		// dirty position - need to send update
		bool isDirty() { return m_dirty; }
		void clear() { m_dirty = false; }
	
	protected:
		int m_NUID; // my network id
		Item* m_item; // my owner
		int m_role; // network role
		NetworkDelegate* m_delegate;
		bool m_dirty;
	};	
}

#endif // !RHEA_NETWORKCOMPONENT_H
