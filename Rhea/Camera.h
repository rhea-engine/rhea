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
			m_graphics_camera(nullptr),
			Item(name, type_name)
		{}

		// positionig
		void setPosition(Poseidon::Position& position);
		void setPosition(Poseidon::Vector3 position);

		// set direction
		void setLookAt(Poseidon::Vector3 look_at);

		// load camera
		void load(ReadyHandler handler);

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		GraphicsCamera* m_graphics_camera;

		bool getGraphicsCamera();
	};
}

#endif // !RHEA_CAMERA_H
