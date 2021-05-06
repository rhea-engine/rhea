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
#include "ItemManager.h"
#include "SceneManager.h"
#include "Item2DManager.h"
#include "TextManager.h"
#include "MaterialManager.h"
#include "CameraManager.h"

namespace Rhea
{
	GraphicsService::GraphicsService(std::string name, std::string type_name) :
		m_platform(nullptr),
		m_viewport_width(0),
		m_viewport_height(0),
		m_g_scene(nullptr),
		Service(name, type_name)	
	{
		m_core = std::make_unique<Poseidon::Core>();

		m_managers["//meworla/scene"] = new GraphicsSceneManager(this);
		m_managers["//meworla/item"] = new GraphicsItemManager(this);
		m_managers["//meworla/character"] = new GraphicsItemManager(this);
		m_managers["//meworla/textwidget"] = new GraphicsTextManager(this);
		m_managers["//meworla/material"] = new GraphicsMaterialManager(this);
		m_managers["//meworla/widget"] = new GraphicsItem2DManager(this);
		m_managers["//meworla/camera"] = new GraphicsCameraManager(this);
	}

	GraphicsManager* GraphicsService::getManager(std::string type_name)
	{
		// find manager for type name in manager list
		auto it = m_managers.find(type_name);
		if (it != m_managers.end()) {
			GraphicsManager* mgr = it->second;
			return mgr;
		}

		return nullptr;
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
		Poseidon::Viewport* viewport = m_core->get_scene_manager()->getViewport();
		Poseidon::RenderContext* context = viewport->createRenderContext(std::string("HUD"));
		context->setStencilState(1);
		// add a camera for this context
		Poseidon::Camera* cam = m_core->createCamera();
		context->addChild(cam);
		context->setCamera(cam);
		m_viewport_width = viewport->getWidth();
		m_viewport_height = viewport->getHeight();
		if (!m_viewport_width)
			m_viewport_width = 800;
		if (!m_viewport_height)
			m_viewport_height = 600;
		cam->setAspectRatio((float)m_viewport_width / (float)m_viewport_height);
		cam->setOrthographicSize((float)m_viewport_height);
		cam->setOrthographic(true);
		cam->setProjectionOffset(Poseidon::Vector3(-0.5f, -0.5f, 0.0f));
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

		if (unit->getTypeName().compare("//meworla/scene")) {
			unit->getApplication()->log("GraphicsService::onDisplay should only be called for scene");
		}

		// try manager
		GraphicsManager* mgr = getManager(unit->getTypeName());
		Element* el;
		if (mgr) {
			el = mgr->createElement(m_core.get(), unit);
		}
		else {
			// default: use item manager
			mgr = getManager("//meworla/item");
			el = mgr->createElement(m_core.get(), unit);			
		}

		if (!unit->getTypeName().compare("//meworla/scene")) {
			m_g_scene = static_cast<GraphicsScene*>(el);
		}

		return ret;
	}

	// factory function
	Unit* GraphicsService::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new GraphicsService(name, type);
		return ret;
	}
}
