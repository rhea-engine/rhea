#include "GraphicsService.h"
#include "GraphicsService.h"
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

#include "Fabrics/3D/Item.h"
#include "Fabrics/3D/Mesh.h"
#include "Poseidon/Renderable.h"
#include "Poseidon/Entity.h"

#include <Psio/Log.h>

namespace Rhea
{
	GraphicsService::GraphicsService(std::string name, std::string type_name) :
		m_platform(nullptr),
		m_viewport_width(0),
		m_viewport_height(0),
		Service(name, type_name)	
	{
		m_core = std::make_unique<Poseidon::Core>();

		m_managers["//meworla/scene"] = new GraphicsSceneManager(this);
		m_managers["//meworla/item"] = new GraphicsItemManager(this);
		m_managers["//meworla/mesh"] = new GraphicsMeshManager(this);
		m_managers["//meworla/character"] = m_managers["//meworla/item"];
		m_managers["//meworla/terrain"] = m_managers["//meworla/item"];
		m_managers["//meworla/material"] = new GraphicsMaterialManager(this);
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

	bool GraphicsService::addElement(Unit* unit)
	{
		// find manager for type name in manager list
		auto it = m_managers.find(unit->getTypeName());
		if (it != m_managers.end()) {
			GraphicsManager* mgr = it->second;
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
		ev->getTarget()->traverse([&](std::string& name, Unit* unit)->bool {
			if (!unit->getElement("graphics")) {
				printf("GraphicsService::onAddItem: add typename: %s\n", unit->getTypeName().c_str());

				// try manager
				GraphicsManager* mgr = getManager(unit->getTypeName());
				if (mgr) {
					mgr->createElement(m_core.get(), unit);
				}
				else if (unit->getValue("isItem")) {
					m_managers["//meworla/item"]->createElement(m_core.get(), unit);
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
				item->traverse([&](std::string name, Unit* u)->bool {
					Item* item = (Item*)u;
				// try manager
				GraphicsManager* mgr = getManager(item->getTypeName());
				if (mgr) {
					mgr->createElement(m_core.get(), item);
				}
				else {
					mgr = getManager("//meworla/item");
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
			onSceneActivate((Scene*)ev->getTarget());
			return false;
			});
		m_app->getUnitTree()->registerEventHandler("scene_deactivate", this, [&](Event* ev, Unit* tree)->bool {
			onSceneDeactivate((Scene*)ev->getTarget());
			return false;
			});
	}
	
	bool GraphicsService::onFrame(float delta)
	{
		// update items
		for (auto it=m_scenes.begin(); it<m_scenes.end(); ++it) {
			(*it)->traverse([](std::string& name, Unit* unit)->bool {
				std::string type_name = unit->getTypeName();
			if ((type_name == "//meworla/item") || (type_name == "//meworla/camera") || (type_name == "//meworla/terrain") || (type_name == "//meworla/character")) {
				Item* item = static_cast<Item*>(unit);
				item->getTransform()->update();
			}
			return true;
				});
		}

		return m_core->iterate();
	}
	
	void GraphicsService::onSceneActivate(Scene* scene)
	{
		if (scene->getTypeName().compare("//meworla/scene")) {
			LOG(LOG_ERROR) << "GraphicsService::onSceneActivate should only be called for scene";
			return;
		}
		m_scenes.push_back(scene);

		// try manager
		GraphicsManager* mgr = getManager(scene->getTypeName());
		Element* el = nullptr;
		if (mgr) {
			el = mgr->createElement(m_core.get(), scene);
		}
		else {
			LOG(LOG_ERROR) << "no graphics scene manager defined.";
			return;
		}

		scene->registerEventHandler("add_item", this, std::bind(&GraphicsService::onAddItem, this, std::placeholders::_1, std::placeholders::_2));
		scene->registerEventHandler("scene_show", this, [&](Event* ev, Unit* unit)->bool {
			Rhea::Scene* scene = static_cast<Scene*>(ev->getTarget());
			if (!scene->getTypeName().compare("//meworla/scene"))
				showScene(scene);
			return false;
			});
		scene->registerEventHandler("scene_hide", this, [&](Event* ev, Unit* unit)->bool {
			Rhea::Scene* scene = static_cast<Scene*>(ev->getTarget());
			if (!scene->getTypeName().compare("//meworla/scene"))
				hideScene(scene);
			return false;
			});
	}

	void GraphicsService::onSceneDeactivate(Scene* scene)
	{
		m_scenes.erase(std::remove(m_scenes.begin(), m_scenes.end(), scene));
		scene->addElement("graphics", nullptr);
		scene->traverse([](std::string& name, Unit* unit)->bool {
			unit->addElement("graphics", nullptr);
			return true;
			});
	}

	void GraphicsService::showScene(Scene* scene)
	{
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
		scene->traverse([scene, scene_mgr](std::string& name, Unit* unit)->bool {
			if (scene == unit)
				return true;
			// we simply assume if the rendercontext is set we must add a render child
			std::string render_context_name = unit->getValue("rendercontext", "");
			if (unit->getValue("isItem") && render_context_name != "") {
				Poseidon::RenderContext* render_context = scene_mgr->getViewport()->getRenderContext(render_context_name);
				if (!render_context) {
					LOG(LOG_ERROR) << "GraphicsService::showScene: render context '" << render_context_name << "' not found";
					return false;
				}
				else {
					render_context->addRenderChild((Item*)unit);
				}
			}
			return true;
			});
	}

	void GraphicsService::hideScene(Scene* scene)
	{
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
		scene->traverse([scene, scene_mgr](std::string& name, Unit* unit)->bool {
			if (scene == unit)
				return true;
			// we simply assume if the rendercontext is set we must add a render child
			std::string render_context_name = unit->getValue("rendercontext", "");
			if (unit->getValue("isItem") && render_context_name != "") {
				Poseidon::RenderContext* render_context = scene_mgr->getViewport()->getRenderContext(render_context_name);
				if (!render_context) {
					LOG(LOG_ERROR) << "GraphicsService::hideScene: render context '" << render_context_name << "' not found";
					return false;
				}
				else {
					render_context->removeRenderChild((Item*)unit);
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
