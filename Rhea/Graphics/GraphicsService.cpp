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

		// try manager
		GraphicsManager* mgr = getManager(unit->getTypeName());
		if (mgr) {
			Element* el = mgr->createElement(m_core.get(), unit);
		}
		else {
			// default: use item manager
			mgr = getManager("//meworla/item");
			Element* el = mgr->createElement(m_core.get(), unit);			
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
