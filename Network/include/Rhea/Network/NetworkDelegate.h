#ifndef RHEA_NETWORKDELEGATE_H
#define RHEA_NETWORKDELEGATE_H

#include <Math/Vector3.h>

namespace rhea
{
	class NetworkDelegate
	{
	public:
		// change the movement of the controlled item
		// for stopping call again with move=0
		virtual void changeMovement(int nuid, math::Vector3 move) = 0;

		// change turn of the controlled item
		// for stopping call again with turn=0
		virtual void changeTurn(int nuid, float turn) = 0;
	};
}

#endif // !RHEA_NETWORKDELEGATE_H
