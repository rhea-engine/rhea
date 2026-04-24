#include <Poseidon/Viewport.h>
#include <Poseidon/Camera.h>
#include <Poseidon/Core.h>
#include <Poseidon/FrameRenderer.h>
#include <Poseidon/RenderContext.h>
#include <Poseidon/RenderWorld.h>

#include <RheaClient/Graphics/CameraManager.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Camera.h>
#include <Psio/Log.h>
#include <RheaClient/Components/RenderComponent.h>

#include <RheaClient/Graphics/GraphicsService.h>
#include "TransformListener.h"

namespace rhea::client {
	bool GraphicsCameraManager::Ensure(poseidon::Core* core, rhea::Unit* unit)
	{
		rhea::Camera* camera = doCast<rhea::Camera>(unit);
		if (!camera) return false;

		// check if already exists
		auto* rc = camera->getComponent<RenderComponent>();

		if (!rc) rc = camera->addComponentUnique<RenderComponent>("renderer", m_next_id++);

		if (rc->hasCamera())
			return true;

		auto* tl = camera->getComponent<TransformListener>();
		if (!tl) {
			tl = camera->addComponentUnique<TransformListener>("transform_listener");
			tl->setGraphicsManager(this);
		}

		// create

		poseidon::FrameRenderer* frame_renderer = core->getFrameRenderer();
		// get dimensions from viewport
		poseidon::Viewport* viewport = frame_renderer->getViewport();
		if (!viewport) {
			LOG(LOG_ERROR) << "No root viewport found - cannot set camera";
			return false;
		}
		float Width = (float)viewport->getWidth();
		float Height = (float)viewport->getHeight();
		if (!Width)
			Width = 800.0f;
		if (!Height)
			Height = 600.0f;

		// add a graphics sys camera
		
		rc->camera = createCamera(core);

		// TODO get viewport from setup

		// get default viewport
		std::string render_context = "default";
		frame_renderer->getViewport()->getRenderContext(render_context)->setCameraHandle(rc->camera);
		
		math::Vector3 m_position;
		Variant pos_sit = unit->getValue("position");
		if (pos_sit != Variant()) {
			float x = pos_sit[0];
			float y = pos_sit[1];
			float z = pos_sit[2];
			m_position = math::Vector3(x, y, z);
		}
		else
			m_position = math::Vector3(0.0f, 0.0f, 0.0f);

		// set a default position
		camera->getTransform()->get_position()->setPosition(m_position);
		camera->getTransform()->get_position()->lookAt(math::Vector3(0.0f, 0.0f, 0.0f), math::Vector3::UNIT_Z);

		math::Quaternion rot_after = camera->getTransform()->get_position()->get_rotation();

		// Check if rotation is valid (not NaN)
		if (std::isnan(rot_after.w) || std::isnan(rot_after.x) || std::isnan(rot_after.y) || std::isnan(rot_after.z)) {
			LOG(LOG_ERROR) << "Camera rotation is NaN! Setting identity quaternion.";
			camera->getTransform()->get_position()->setOrientation(math::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		}

		syncCameraTransform(core, rc->camera, camera->getTransform());
		syncCameraProjection(core, rc->camera, camera, Width, Height);

		// register handlers
		unit->registerEventHandler("delete", this, std::bind(&GraphicsCameraManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));
		unit->registerEventHandler("update", this, std::bind(&GraphicsCameraManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2));

		return true;
	}

	bool GraphicsCameraManager::onUpdate(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Camera* camera = doCast<rhea::Camera>(unit);
		if (!camera)
			return false;

		auto* rc = camera->getComponent<RenderComponent>();
		if (!rc || !rc->hasCamera()) {
			return Ensure(m_service->getEngine(), unit);
		}

		poseidon::FrameRenderer* frame_renderer = m_service->getEngine()->getFrameRenderer();
		poseidon::Viewport* viewport = frame_renderer ? frame_renderer->getViewport() : nullptr;

		float width = viewport ? (float)viewport->getWidth() : 800.0f;
		float height = viewport ? (float)viewport->getHeight() : 600.0f;
		if (!width) width = 800.0f;
		if (!height) height = 600.0f;

		syncCameraTransform(m_service->getEngine(), rc->camera, camera->getTransform());
		syncCameraProjection(m_service->getEngine(), rc->camera, camera, width, height);

		return false;
	}

	void GraphicsCameraManager::onTransformUpdate(rhea::Unit* unit)
	{
		rhea::Camera* camera = doCast<rhea::Camera>(unit);
		if (!camera)
			return;
		auto rc = camera->getComponent<RenderComponent>();
		if (!rc) return;
		syncCameraTransform(m_service->getEngine(), rc->camera, camera->getTransform());
	}

	void GraphicsCameraManager::onFrame()
	{
	}

	bool GraphicsCameraManager::onItemDelete(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Camera* item = doCast<rhea::Camera>(unit);
		if (!item) return true;

		auto* rc = item->getComponent<RenderComponent>();
		if (!rc) return true;

		if (rc->ctx) {
			if (rc->ctx->getCameraHandle() == rc->camera) {
				rc->ctx->setCameraHandle(poseidon::InvalidCamera);				
			}
		}

		if (rc->hasCamera()) {
			m_service->getEngine()->getRenderWorld()->destroyCamera(rc->camera);

			rc->camera = poseidon::InvalidCamera;
		}

		return false;
	}

	void GraphicsCameraManager::syncCameraTransform(poseidon::Core* core, poseidon::CameraHandle handle, rhea::Transform* transform)
	{
		if (!core || handle == poseidon::InvalidCamera || !transform)
			return;

		poseidon::Camera* cam = core->getRenderWorld()->resolveCamera(handle);
		if (!cam)
			return;

		auto* derived = transform->get_derived_position();
		if (!derived)
			return;

		const math::Vector3& pos = derived->get_worldspace();
		const math::Quaternion& rot = derived->get_rotation();

		cam->setPosition(pos);
		cam->setOrientation(rot);		
	}

	void GraphicsCameraManager::syncCameraProjection(
		poseidon::Core* core,
		poseidon::CameraHandle handle,
		rhea::Camera* sourceCamera,
		float viewport_width,
		float viewport_height)
	{
		if (!core || handle == poseidon::InvalidCamera)
			return;

		poseidon::Camera* cam = core->getRenderWorld()->resolveCamera(handle);
		if (!cam)
			return;

		if (viewport_width <= 0.0f)  viewport_width = 800.0f;
		if (viewport_height <= 0.0f) viewport_height = 600.0f;

		cam->setAspectRatio(viewport_width / viewport_height);
		cam->setOrthographicSize(viewport_height);

		// add these once Rhea camera exposes them cleanly
		// if (sourceCamera) {
		// cam->setOrthographic(sourceCamera->isOrthographic());
		// cam->setOrthographicSize(sourceCamera->getOrthographicSize());
		// cam->setNearClip(sourceCamera->getNearClip());
		// cam->setFarClip(sourceCamera->getFarClip());
		// cam->setFov(sourceCamera->getFov());
		// }
	}
}
