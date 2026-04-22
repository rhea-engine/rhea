#include "LightManager.h"
#include <Psio/Log.h>
#include <Rhea/Core/DirectionalLight.h>
#include <Rhea/Core/PointLight.h>
#include <Rhea/Core/SpotLight.h>
#include <Poseidon/Light.h>
#include <Poseidon/RenderWorld.h>
#include "RheaClient/Graphics/GraphicsService.h"

namespace rhea::client
{
	GraphicsLightManager::~GraphicsLightManager()
	{
	}
	
	bool GraphicsLightManager::Ensure(poseidon::Core* core, rhea::Unit* unit)
	{
		rhea::Light* light = doCast<rhea::Light>(unit);
		if (!light) return false;

		// register event handlers for delete and update - these will be called even if the item is not renderable, so we can catch changes to renderable state and mesh assignment
		static auto deleteHandler = std::bind(&GraphicsLightManager::onLightDelete, this, std::placeholders::_1, std::placeholders::_2);
		static auto updateHandler = std::bind(&GraphicsLightManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2);

		if (!unit->hasEventHandler("delete", this)) {
			unit->registerEventHandler("delete", this, deleteHandler);
		}
		if (!unit->hasEventHandler("update", this)) {
			unit->registerEventHandler("update", this, updateHandler);
		}

		auto* rc = light->getComponent<RenderComponent>();
		if (!rc) rc = light->addComponentUnique<RenderComponent>("renderer");

		if (rc->hasLight())
			return true;

		DLOG(LOG_DEBUG) << "GraphicsItemManager::Ensure: registering light";
		poseidon::RenderContext* context = getRenderContext(core, "default");
		if (!context) {
			LOG(LOG_ERROR) << "GraphicsLightManager::Ensure: default render context not found";
			return false;
		}

		rc->ctx = context;
		rc->light = createLight(core);
		if (!rc->hasLight()) {
			LOG(LOG_ERROR) << "GraphicsLightManager::Ensure: createLight returned invalid handle";
			return false;
		}
		rc->ctx->addLight(rc->light);

		syncLight(core, rc->light, light);
		return true;
	}

	bool GraphicsLightManager::onUpdate(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Light* light = doCast<rhea::Light>(unit);
		if (!light) return false;

		auto* rc = light->getComponent<RenderComponent>();
		if (!rc) {
			// no render component yet -> ensure registration now
			return Ensure(m_service->getEngine(), unit);
		}

		// If for some reason the light lost its registration, re-create it
		if (!rc->hasLight()) {
			return Ensure(m_service->getEngine(), unit);
		}

		// If context went missing, restore it
		if (!rc->ctx) {
			rc->ctx = getRenderContext(m_service->getEngine(), "default");
			if (rc->ctx) {
				rc->ctx->addLight(rc->light);
			}
		}

		syncLight(m_service->getEngine(), rc->light, light);
		return true;
	}

	bool GraphicsLightManager::onLightDelete(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Light* light = doCast<rhea::Light>(unit);
		if (!light) return true;

		auto* rc = light->getComponent<RenderComponent>();
		if (!rc) return true;

		if (rc->hasLight()) {
			if (rc->ctx) {
				rc->ctx->removeLight(rc->light);
			}

			m_service->getEngine()->getRenderWorld()->destroyLight(rc->light);
			rc->light = poseidon::InvalidLight;
		}

		rc->ctx = nullptr;
		return false;
	}

	void GraphicsLightManager::syncLight(
		poseidon::Core* core,
		poseidon::LightHandle handle,
		rhea::Light* source)
	{
		if (!core || !source || handle == poseidon::InvalidLight)
			return;

		poseidon::Light* dst = core->getRenderWorld()->resolveLight(handle);
		if (!dst)
			return;

		dst->setVisible(true);
		dst->setColor(source->getColor());
		dst->setDiffuse(source->getDiffuse());
		dst->setSpecular(source->getSpecular());

		if (auto* dir = doCast<rhea::DirectionalLight>(source)) {
			dst->setType(poseidon::Light::LightType::Directional);
			dst->setDirection(dir->getDirection());
		}
		else if (auto* point = doCast<rhea::PointLight>(source)) {
			dst->setType(poseidon::Light::LightType::Point);
			auto pos = point->getTransform()->get_position()->get_worldspace();
			dst->setPosition(math::Vector3(pos.x, pos.y, pos.z));
			dst->setAttenuation(point->getAttenuation());
		}
		else if (auto* spot = doCast<rhea::SpotLight>(source)) {
			dst->setType(poseidon::Light::LightType::Spot);

			auto pos = spot->getTransform()->get_position()->get_worldspace();
			math::Vector3 p(pos.x, pos.y, pos.z);
			dst->setPosition(p);

			// replace with a proper transform-derived forward direction if you have it
			math::Vector3 dir = -p;
			dir.normalize();
			dst->setDirection(dir);

			dst->setAttenuation(spot->getAttenuation());
			dst->setConeAngle(spot->getConeAngle());
		}
	}
}