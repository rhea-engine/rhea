#ifndef RHEA_FABRICS_SKYBOX_H
#define RHEA_FABRICS_SKYBOX_H

#include "../Core/Item.h"
#include "Camera.h"

namespace Rhea
{
	class Skybox : public Item
	{
		MECLASS(Skybox, Item)
	public:
		Skybox(std::string name, std::string type_name) :
			m_camera(nullptr),
			Item(name, type_name)
		{}
	
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		std::vector<Unit::Export> getReferences();

		void ready() override;	

		// on Frame event
		void Skybox::onFrame(float delta) override;

	protected:
		Camera* m_camera;
	};
}
#endif // !RHEA_FABRICS_SKYBOX_H
