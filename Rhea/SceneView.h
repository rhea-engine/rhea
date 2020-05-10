#ifndef RHEA_SCENEVIEW_H
#define RHEA_SCENEVIEW_H

#include "Item.h"

namespace Poseidon
{
	class Core;
	class Camera;
	class Viewport;
}

namespace Rhea
{
	// a class tha holds a view to the scene
	class SceneView : public Item
	{
	public:
		SceneView(std::string path_name, std::string type_name) :
			m_camera(NULL),
			m_viewport(NULL),
			Item(path_name, type_name)
		{}

		// positionig
		void setPosition(Poseidon::Position& position);
		void setPosition(Poseidon::Vector3 position);

		// set direction
		void setLookAt(Poseidon::Vector3 look_at);

		// load camera
		void load(ReadyHandler handler);

		// show in scene
		void show(Poseidon::Core* core);

		Poseidon::Node* getNode();

		void message(std::string msg);

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		Poseidon::Viewport* m_viewport;
		Poseidon::Camera* m_camera;
	};
}
#endif // !RHEA_SCENEVIEW_H
