#ifndef RHEA_CAMERA_H
#define RHEA_CAMERA_H

#include "Item.h"

#include "Poseidon/Position.h"

namespace Rhea
{
	class GraphicsCamera;

	class Camera : public Item
	{
	public:
		Camera(std::string name, std::string type_name) :			
			Item(name, type_name)
		{}

		// load camera
		void load(ReadyHandler handler);

		// factory function
		static Unit* createType(std::string name, std::string type);
	};
}

#endif // !RHEA_CAMERA_H
