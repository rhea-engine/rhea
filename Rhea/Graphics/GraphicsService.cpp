#include <functional>

#include "GraphicsService.h"

#include "../Platform/PlatformService.h"
#include "../Application.h"
#include "../Unit.h"
#include "../Scene.h"
#include "../UnitTree.h"

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
#include <Rhea/Physics/PhysicsService.h>
#include "EventInput.h"

#include "../Item.h"
#include "../Mesh.h"
#include "Poseidon/Renderable.h"
#include "Poseidon/Entity.h"

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

		// register input handler
		m_app->getUnitTree()->registerEventHandler("scene_input", std::bind(&GraphicsService::onInput, this, std::placeholders::_1, std::placeholders::_2));
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

	bool GraphicsService::onInput(Event* ev, Unit* unit)
	{
		if (ev->isEvent("input")) {
			EventInput* e = static_cast<EventInput*>(ev);
			std::shared_ptr<Psio::MouseInputEvent> me = std::static_pointer_cast<Psio::MouseInputEvent>(e->getEvent());

			switch (e->getEvent()->getId()) {
			case Psio::InputEventId::MOUSE_MOVE:				
				m_mx = me->getX();
				m_my = me->getY();				
				break;
			case Psio::InputEventId::MOUSE_BTN_PRESSED:
				Unit* picked = pick();
				if (picked)
					picked->sendEvent("scene_input", ev);
			}
		}
		return false;
	}

	// factory function
	Unit* GraphicsService::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new GraphicsService(name, type);
		return ret;
	}
	
	Unit* GraphicsService::pick()
	{
		static Poseidon::Mesh* m_mesh = nullptr;

		// ray tracing to current hover object
		PhysicsService* phys = static_cast<PhysicsService*>(m_app->getService("physics"));
		std::string def = "default";
		Poseidon::Camera* camera = m_core->get_scene_manager()->getViewport()->getRenderContext(def)->get_camera();
		if (phys && camera) {
			
			// compute the ray
			Poseidon::Ray ray = camera->getPickingRay(m_mx, m_my, m_viewport_width, m_viewport_height);

			Poseidon::Vector3 ray_from = ray.getOrigin();
			Poseidon::Vector3 ray_dir = ray.getDirection();
			ray_dir *= 1000.0f;
			Poseidon::Vector3 ray_to = ray.getOrigin() + ray_dir;

			// call ray test from physics service
			return phys->ray(ray_from, ray_to);

#ifdef PICK3D_DEBUG
			if (!m_mesh) {
				// prepare debug item

				m_mesh = new Poseidon::Mesh();
				m_mesh->setViewport(m_app->get_core()->get_scene_manager()->getViewport());
				Poseidon::Entity* entity = m_core->createEntity(m_mesh);
				entity->setName("ray_test");
				Poseidon::Material* material = m_app->get_core()->getMaterial("color");
				entity->setMaterial(material);
				m_app->get_core()->get_scene_manager()->getViewport()->getRenderContext(std::string("default"))->addRenderChild(entity);
			}

			std::vector<Hippios::vertex> l_vertices;		
			Poseidon::Vector3 pos = ray.getOrigin();
			Poseidon::Vector3 dir = ray_to;

			//dir *= 1000.0f;

			l_vertices.push_back({ pos.x, pos.y, pos.z, 0.0f, 1.0f, 0.0f, 1.0f });
			l_vertices.push_back({ dir.x, dir.y, dir.z, 0.0f, 1.0f, 0.0f, 1.0f });
			m_mesh->setVertices(sizeof(Hippios::vertex), l_vertices.size(), l_vertices.data());

			m_app->log("pos: " + std::to_string(pos.x) + "/" + std::to_string(pos.y) + "/" + std::to_string(pos.z));
			m_app->log("ray: " + std::to_string(dir.x) + "/" + std::to_string(dir.y) + "/" + std::to_string(dir.z));

			Poseidon::Vector3 deb1 = camera->getProjectionMatrixRS() * camera->getViewMatrix() * pos;
			Poseidon::Vector3 deb2 = camera->getProjectionMatrixRS() * camera->getViewMatrix() * dir;

			m_app->log("deb1: " + std::to_string(deb1.x) + "/" + std::to_string(deb1.y) + "/" + std::to_string(deb1.z));
			m_app->log("deb2: " + std::to_string(deb2.x) + "/" + std::to_string(deb2.y) + "/" + std::to_string(deb2.z));
#endif
		}
		return nullptr;
	}
}
