#include <Rhea/Controller/FreeMovementComponent.h>
#include <Psio/Log.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Item.h>

namespace rhea
{
	void FreeMovementComponent::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values)
	{
		Component::setup(app, attributes, values);
		// register as component if not already registered
		if (m_parent && !m_parent->getComponent<FreeMovementComponent>()) {
			m_parent->addComponent(this);
		}
	}

    void FreeMovementComponent::onFrame(float delta_time)
    {
        auto* item = doCast<Item>(m_parent);
        if (!item || delta_time <= 0.0f)
            return;

        const math::Vector3 movement = item->getMovement();
        const float turn = item->getTurn();

        if (movement == math::Vector3::ZERO && turn == 0.0f)
            return;

        Position position = item->getTransform()->getWorldPosition();

        // Movement is expressed in the item's local directions.
        const math::Vector3 displacement = position.getRotation() * (movement * delta_time);
        position.setPosition(position.getTranslation() + displacement);

        // Turn around the world up axis; turn is in radians per second.
        if (turn != 0.0f) {
            math::Quaternion rotation;
            rotation.fromAngleAxis(turn * delta_time, math::Vector3(0.0f, 1.0f, 0.0f));

            math::Quaternion orientation = rotation * position.getRotation();
            orientation.normalize();
            position.setRotation(orientation);
        }

        item->setWorldPosition(position);
    }
}