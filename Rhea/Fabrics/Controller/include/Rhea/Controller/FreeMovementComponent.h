#ifndef RHEA_FREEMOVEMENTCOMPONENT_H
#define RHEA_FREEMOVEMENTCOMPONENT_H

#include <Rhea/Components/ActiveComponent.h>

namespace rhea
{
	class FreeMovementComponent : public ActiveComponent
	{
		RECLASS(FreeMovementComponent, ActiveComponent, "//meworla/free_movement");
	public:
		FreeMovementComponent(std::string name, std::string type_name) :
			ActiveComponent(name, type_name)
		{}
		virtual ~FreeMovementComponent() = default;
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values) override;
		void onFrame(float delta_time) override;
	private:
		float m_speed = 1.0f;
	};
}

#endif // !RHEA_FREEMOVEMENTCOMPONENT_H