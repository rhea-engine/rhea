#include "GraphicsService.h"
#include <functional>

#include "../Platform/PlatformService.h"
#include "../Application.h"
#include "../Unit.h"
#include "Fabrics/Core//Scene.h"
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

#include "MaterialManager.h"
#include "CameraManager.h"
#include "MeshManager.h"
#include <Rhea/Physics/PhysicsService.h>
#include "EventInput.h"

#include "Fabrics/Core/Item.h"
#include "Fabrics/3D/Mesh.h"
#include "Poseidon/Renderable.h"
#include "Poseidon/Entity.h"

#include <Rhea/Fabrics/Core/Material.h>
#include <Rhea/Fabrics/3D/Camera.h>

#include <Psio/Log.h>

namespace Rhea
{
	GraphicsService::GraphicsService(std::string name, std::string type_name) :
		m_platform(nullptr),
		m_viewport_width(0),
		m_viewport_height(0),
		m_mx(0),
		m_my(0),
		Service(name, type_name)	
	{
		m_core = std::make_unique<Poseidon::Core>();

		addManager<Scene>(std::make_unique<GraphicsSceneManager>(this));
		addManager<Item>(std::make_unique<GraphicsItemManager>(this));
		addManager<Mesh>(std::make_unique<GraphicsMeshManager>(this));
		addManager<Material>(std::make_unique<GraphicsMaterialManager>(this));
		addManager<Camera>(std::make_unique<GraphicsCameraManager>(this));
	}

	GraphicsService::~GraphicsService()
	{		
	}

	GraphicsManager* GraphicsService::getManager(SimpleParser::Type* type)
	{
		// find manager for type name in manager list
		auto it = m_managers.find(type);
		if (it != m_managers.end()) {
			GraphicsManager* mgr = it->second.get();
			return mgr;
		}		
		
		return nullptr;
	}

	bool GraphicsService::addElement(Unit* unit)
	{
		// find manager for type name in manager list
		auto it = m_managers.find(unit->getType());
		if (it != m_managers.end()) {
			GraphicsManager* mgr = it->second.get();
			mgr->createElement(m_core.get(), unit);
			return true;
		}
		return false;
	}

	void GraphicsService::setPlatform(Service* platform_service)
	{
		m_platform = platform_service;
	}

	bool GraphicsService::onAddItem(Event* ev, Unit* unit)
	{
		ev->getTarget()->traverse([&](std::string& name, Component* unit)->bool {
			Unit* u = doCast<Unit>(unit);
			if (u && !u->getComponent<GraphicsComponent>()) {
				printf("GraphicsService::onAddItem: add typename: %s\n", unit->getTypeName().c_str());

				// try manager
				GraphicsManager* mgr = getManager(unit->getType());
				if (mgr) {
					mgr->createElement(m_core.get(), u);
				}
				else if (doCast<Item>(unit)) {
					getManager<Item>()->createElement(m_core.get(), u);
				}
			}
		return true;
			});
		return false;
	}

	void GraphicsService::addItem(std::string render_context, Item* item)
	{
		Poseidon::Viewport* viewport = m_core->get_scene_manager()->getViewport();
		if (viewport) {
			Poseidon::RenderContext* context = viewport->getRenderContext(render_context);
			if (context) {
				context->addRenderChild(item);
				// add elements
				item->traverse([&](std::string name, Component* u)->bool {
					Item* item = doCast<Item>(u);
					if (!item)
						return true;
				// try manager
				GraphicsManager* mgr = getManager(item->getType());
				if (mgr) {
					mgr->createElement(m_core.get(), item);
				}
				else {
					mgr = getManager<Item>();
					mgr->createElement(m_core.get(), item);
				}
				return true;
					});
			}
			else
				abort();
		}
		else
			abort();
	}

	void GraphicsService::onInit()
	{
		if (!m_platform) {
			LOG(LOG_ERROR) << "GraphicsService::onInit: platform not set";
			return;
		}
		else
			m_core->init(static_cast<PlatformService*>(m_platform)->getEngine());

		// create a render context for HUD / GUI
		Poseidon::Viewport* viewport = m_core->get_scene_manager()->getViewport();
		Poseidon::RenderContext* context = viewport->createRenderContext(std::string("HUD"));
		context->setStencilState(1);
		// add a camera for this context
		Poseidon::Camera* cam = m_core->createCamera();
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
		cam->setProjectionOffset(Math::Vector3(-0.5f, -0.5f, 0.0f));
		cam->updateProjection();
		cam->updateView();

		// register input handler
		m_app->getUnitTree()->registerEventHandler("scene_input", this, std::bind(&GraphicsService::onInput, this, std::placeholders::_1, std::placeholders::_2));
		m_app->getUnitTree()->registerEventHandler("scene_activate", this, [&](Event* ev, Unit* tree)->bool {
			onSceneActivate(doCast<Scene>(ev->getTarget()));
			return false;
			});
		m_app->getUnitTree()->registerEventHandler("scene_deactivate", this, [&](Event* ev, Unit* tree)->bool {
			onSceneDeactivate(doCast<Scene>(ev->getTarget()));
			return false;
			});
	}
	
	bool GraphicsService::onFrameFinal(float delta)
	{
		return m_core->iterate();
	}
	
	void GraphicsService::onSceneActivate(Scene* scene)
	{
		if (!scene) {
			LOG(LOG_ERROR) << "No scene object was provided to onSceneActivate";
			return;
		}

		m_scenes.push_back(scene);

		// try manager
		GraphicsManager* mgr = getManager(scene->getType());
		GraphicsElement* el = nullptr;
		if (mgr) {
			el = mgr->createElement(m_core.get(), scene);
		}
		else {
			LOG(LOG_ERROR) << "no graphics scene manager defined.";
			return;
		}

		scene->registerEventHandler("add_item", this, std::bind(&GraphicsService::onAddItem, this, std::placeholders::_1, std::placeholders::_2));
		scene->registerEventHandler("scene_show", this, [&](Event* ev, Unit* unit)->bool {
			showScene(doCast<Scene>(ev->getTarget()));
			return false;
			});
		scene->registerEventHandler("scene_hide", this, [&](Event* ev, Unit* unit)->bool {
			hideScene(doCast<Scene>(ev->getTarget()));
			return false;
			});
	}

	void GraphicsService::onSceneDeactivate(Scene* scene)
	{
		if (!scene) {
			LOG(LOG_ERROR) << "No scene object was provided to onSceneDeactivate";
			return;
		}

		m_scenes.erase(std::remove(m_scenes.begin(), m_scenes.end(), scene));
		scene->deleteComponent<GraphicsComponent>();
		scene->traverse([](std::string& name, Component* unit)->bool {
			Unit* u = doCast<Unit>(unit);
			if (u)
				u->deleteComponent<GraphicsComponent>();
			return true;
			});
	}

	void GraphicsService::showScene(Scene* scene)
	{
		if (!scene) {
			LOG(LOG_ERROR) << "No scene object was provided to showScene";
			return;
		}

		Poseidon::SceneManager* scene_mgr = m_core->get_scene_manager();
		if (!scene_mgr) {
			LOG(LOG_ERROR) << "GraphicsService::showScene: no scene manager";
			return;
		}
		std::string render_context_name = scene->getValue("rendercontext", "default");
		Poseidon::RenderContext* render_context = scene_mgr->getViewport()->getRenderContext(render_context_name);
		if (!render_context) {
			LOG(LOG_ERROR) << "GraphicsService::showScene: render context '" << render_context_name << "' not found";
			return;
		}
		render_context->addRenderChild((Item*)scene);

		// scene children with other contexts
		// handle all current subtree nodes
		scene->traverse([scene, scene_mgr](std::string& name, Component* unit)->bool {
			if (scene == unit)
				return true;
			Item* item = doCast<Item>(unit);
			if (!item)
				return true;
			// we simply assume if the rendercontext is set we must add a render child
			std::string render_context_name = item->getValue("rendercontext", "");
			if (render_context_name != "") {
				Poseidon::RenderContext* render_context = scene_mgr->getViewport()->getRenderContext(render_context_name);
				if (!render_context) {
					LOG(LOG_ERROR) << "GraphicsService::showScene: render context '" << render_context_name << "' not found";
					return false;
				}
				else {
					render_context->addRenderChild(item);
				}
			}
			return true;
			});
	}

	void GraphicsService::hideScene(Scene* scene)
	{
		if (!scene) {
			LOG(LOG_ERROR) << "No scene object was provided to hideScene";
			return;
		}

		Poseidon::SceneManager* scene_mgr = m_core->get_scene_manager();
		if (!scene_mgr) {
			LOG(LOG_ERROR) << "GraphicsService::hideScene: no scene manager";
			return;
		}
		std::string render_context_name = scene->getValue("rendercontext", "default");
		Poseidon::RenderContext* render_context = scene_mgr->getViewport()->getRenderContext(render_context_name);
		if (!render_context) {
			LOG(LOG_ERROR) << "GraphicsService::hideScene: render context '" << render_context_name << "' not found";
			return;
		}
		render_context->removeRenderChild((Item*)scene);

		// scene children with other contexts
		// handle all current subtree nodes
		scene->traverse([scene, scene_mgr](std::string& name, Component* unit)->bool {
			if (scene == unit)
				return true;
			Item* item = doCast<Item>(unit);
			if (!item)
				return true;
			// we simply assume if the rendercontext is set we must add a render child
			std::string render_context_name = item->getValue("rendercontext", "");
			if (render_context_name != "") {
				Poseidon::RenderContext* render_context = scene_mgr->getViewport()->getRenderContext(render_context_name);
				if (!render_context) {
					LOG(LOG_ERROR) << "GraphicsService::hideScene: render context '" << render_context_name << "' not found";
					return false;
				}
				else {
					render_context->removeRenderChild(item);
				}
			}
			return true;
			});
	}

	bool GraphicsService::onInput(Event* ev, Unit* unit)
	{
		if (ev->isEvent("input")) {
			EventInput* e = static_cast<EventInput*>(ev);
			std::shared_ptr<Psio::MouseInputEvent> me = std::static_pointer_cast<Psio::MouseInputEvent>(e->getEvent());
			int viewport_width;
			int viewport_height;
			std::string context_name;
			Unit* picked;

			switch (e->getEvent()->getId()) {
			case Psio::InputEventId::MOUSE_MOVE:				
				m_mx = me->getX();
				m_my = me->getY();				
				break;
			case Psio::InputEventId::MOUSE_BTN_PRESSED:
				picked = pick();
				if (picked)
					picked->sendEvent("scene_input", ev);
				break;

			case Psio::InputEventId::WIN_SIZE:
				// resize window
				/*
				std::shared_ptr<Psio::WinSizeEvent> me = std::static_pointer_cast<Psio::WinSizeEvent>(e->getEvent());
				viewport_width = me->width();
				viewport_height = me->height();
				if (viewport_width != m_viewport_width || viewport_height != m_viewport_height) {
					float ratio = (float)m_viewport_width / (float)m_viewport_height;
					if (ratio < (800.0f / 600.f)) {
						viewport_height = (float)m_viewport_width / (800.0f / 600.f);
					}
					else {
						viewport_width = (float)m_viewport_height / (800.0f / 600.f);
					}
					
					m_viewport_wbidth = viewport_width;
					m_viewport_height = viewport_height;

					Poseidon::Viewport* viewport = m_core->get_scene_manager()->getViewport();
					context_name = "HUD";
					Poseidon::RenderContext* context = viewport->getRenderContext(context_name);
					if (0 && context) {
						Poseidon::Camera* cam = context->get_camera();
						//cam->setAspectRatio((float)m_viewport_width / (float)m_viewport_height);
						cam->setAspectRatio((800.0f / 600.f));
						cam->setOrthographicSize((float)m_viewport_height);			
						cam->updateProjection();
						cam->updateView();
					}
				}
				*/
				break;
			}
		}
		return false;
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
			Math::Ray ray = camera->getPickingRay((float)m_mx, (float)m_my, m_viewport_width, m_viewport_height);

			Math::Vector3 ray_from = ray.getOrigin();
			Math::Vector3 ray_dir = ray.getDirection();
			ray_dir *= 1000.0f;
			Math::Vector3 ray_to = ray.getOrigin() + ray_dir;

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
			Math::Vector3 pos = ray.getOrigin();
			Math::Vector3 dir = ray_to;

			//dir *= 1000.0f;

			l_vertices.push_back({ pos.x, pos.y, pos.z, 0.0f, 1.0f, 0.0f, 1.0f });
			l_vertices.push_back({ dir.x, dir.y, dir.z, 0.0f, 1.0f, 0.0f, 1.0f });
			m_mesh->setVertices(sizeof(Hippios::vertex), l_vertices.size(), l_vertices.data());

			DLOG(LOG_DEBUG) << "pos: " << std::to_string(pos.x) << "/" + std::to_string(pos.y) << "/" << std::to_string(pos.z);
			DLOG(LOG_DEBUG) << "ray: " << std::to_string(dir.x) << "/" + std::to_string(dir.y) << "/" + std::to_string(dir.z);

			Math::Vector3 deb1 = camera->getProjectionMatrixRS() * camera->getViewMatrix() * pos;
			Math::Vector3 deb2 = camera->getProjectionMatrixRS() * camera->getViewMatrix() * dir;

			DLOG(LOG_DEBUG) << "deb1: " << std::to_string(deb1.x) << "/" + std::to_string(deb1.y) << "/" + std::to_string(deb1.z);
			DLOG(LOG_DEBUG) << "deb2: " << std::to_string(deb2.x) << "/" + std::to_string(deb2.y) << "/" + std::to_string(deb2.z);
#endif
		}
		return nullptr;
	}
}
