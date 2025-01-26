#ifndef RHEA_CAMERA_H
#define RHEA_CAMERA_H

#include "../Core/Item.h"

#include "../Core/Position.h"

namespace Rhea
{
	class GraphicsCamera;

	class Camera : public Item
	{
		MECLASS(Camera, Item)
	public:
		Camera(std::string name, std::string type_name) :			
			Item(name, type_name)
		{}
	};
}

#endif // !RHEA_CAMERA_H
