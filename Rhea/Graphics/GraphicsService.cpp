#include <functional>

#include "GraphicsService.h"

#include "../Platform/PlatformService.h"
#include "../Application.h"
#include "../Unit.h"
#include "../Scene.h"

#include "Poseidon/Core.h"
#include "Poseidon/Viewport.h"
#include "Poseidon/SceneManager.h"
#include "Poseidon/RenderContext.h"
#include "Poseidon/Camera.h"

#include "../Element.h"
#include "Texture.h"
#include "Mesh.h"
#include "Item.h"
#include "Scene.h"
#include "Item2D.h"

namespace Rhea
{
	GraphicsService::GraphicsService(Application* app) :
		m_platform(nullptr),
		Service(app, "GraphicsService")	
	{
		m_core = std::make_unique<Poseidon::Core>();
		m_element_factory["//meworla/texture"] = &GraphicsTexture::createElement;
		m_element_factory["//meworla/mesh"] = &GraphicsMesh::createElement;
		m_element_factory["//meworla/item"] = &GraphicsItem::createElement;
		m_element_factory["//meworla/scene"] = &GraphicsScene::createElement;
		m_element_factory["//meworla/widget"] = &GraphicsItem2D::createElement;
	}

	void GraphicsService::setPlatform(Service* platform_service)
	{
		m_platform = platform_service;
	}

	void GraphicsService::setScene(Unit* scene)
	{
		scene->registerEventHandler("display", std::bind(&GraphicsService::onDisplay, this, std::placeholders::_1, std::placeholders::_2));
	}

	void GraphicsService::onInit()
	{
		if (!m_platform)
			m_app->log("GraphicsService::onInit: platform not set");
		else
			m_core->init(static_cast<PlatformService*>(m_platform)->getEngine());

		// create a render context for HUD / GUI
		Poseidon::RenderContext* context = m_core->get_scene_manager()->getViewport()->createRenderContext(std::string("HUD"));
		// add a camera for this context
		Poseidon::Camera* cam = m_core->createCamera();
		context->addChild(cam);
		context->setCamera(cam);
		float Width = 800;
		float Height = 600;
		cam->setAspectRatio((float)Width / (float)Height);
		cam->setOrthographicSize(600);
		cam->setOrthographic(true);
		cam->updateProjection();
		cam->updateView();
	}
	
	bool GraphicsService::onFrame()
	{
		return m_core->iterate();
	}
	
	bool GraphicsService::onDisplay(Event* ev, Unit* unit)
	{
		bool ret = false;

		// create type if factory found
		auto it = m_element_factory.find(unit->getType());
		if (it != m_element_factory.end()) {
			Element* el = it->second(m_core.get(), unit);
		}

		return ret;
	}
}
