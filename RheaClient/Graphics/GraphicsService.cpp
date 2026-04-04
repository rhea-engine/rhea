#include <Rhea/Graphics/GraphicsService.h>
#include <functional>

#include <Rhea/Platform/PlatformService.h>
#include <Rhea/Application.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Scene.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Graphics/SizeEvent.h>

#include "Poseidon/Core.h"
#include "Poseidon/Viewport.h"
#include "Poseidon/SceneManager.h"
#include "Poseidon/RenderContext.h"
#include "Poseidon/Camera.h"
#include "Poseidon/Window.h"

#include <Rhea/Element.h>
#include <Rhea/Graphics/Texture.h>
#include "ItemManager.h"
#include "SceneManager.h"

#include <Rhea/Graphics/MaterialManager.h>
#include <Rhea/Graphics/CameraManager.h>
#include "MeshManager.h"

#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/EventInput.h>

#include <Rhea/Core/Item.h>
#include <Rhea/Core/Mesh.h>
#include "Poseidon/Renderable.h"
#include "Poseidon/Entity.h"

#include <Rhea/Core/Material.h>
#include <Rhea/Core/Camera.h>

#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/ConfigFile.h>

namespace rhea::client {
	GraphicsService::GraphicsService(std::string name, std::string type_name) :
		m_platform(nullptr),
		m_mx(0),
		m_my(0),
		m_fullscreen(false),
		rhea::IGraphicsService(name, type_name)	
	{
		m_core = std::make_unique<Poseidon::Core>();

		addManager<rhea::Scene>(std::make_unique<GraphicsSceneManager>(this));
		addManager<rhea::Item>(std::make_unique<GraphicsItemManager>(this));
		addManager<rhea::Mesh>(std::make_unique<GraphicsMeshManager>(this));
		addManager<rhea::Material>(std::make_unique<GraphicsMaterialManager>(this));
		addManager<rhea::Camera>(std::make_unique<GraphicsCameraManager>(this));
	}

	GraphicsService::~GraphicsService()
	{		
	}

	void GraphicsService::ToggleFullscreen()
	{
		if (!m_fullscreen)
			m_core->SetFullscreen(true);
		else
			m_core->SetFullscreen(false);
		m_fullscreen = !m_fullscreen;
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

	bool GraphicsService::addElement(rhea::Unit* unit)
	{
		// find manager for type name in manager list
		auto it = m_managers.find(unit->getType());
		if (it != m_managers.end()) {
			GraphicsManager* mgr = it->second.get();
			mgr->Ensure(m_core.get(), unit);
			return true;
		}
		return false;
	}

	void GraphicsService::setPlatform(rhea::Service* platform_service)
	{
		m_platform = platform_service;
	}

	bool GraphicsService::onAddItem(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Unit* root = doCast<rhea::Unit>(ev->getTarget());
		if (!root) return false;

		root->traverse([&](std::string& name, rhea::Component* c)->bool {

			if (rhea::Item* item = doCast<rhea::Item>(c)) {
				getManager<rhea::Item>()->Ensure(m_core.get(), item);
				return true;
			}

			// we do not create assets here - could be added if we want eager creation

			return true;
			});

		return false;

	}

	void GraphicsService::addItem(std::string render_context, rhea::Item* item)
	{
		Poseidon::Viewport* viewport = m_core->get_scene_manager()->getViewport();
		if (!viewport) abort();

		Poseidon::RenderContext* context = viewport->getRenderContext(render_context);
		if (!context) abort();

		context->addRenderChild(item);

		// Create instance elements for all items in subtree
		item->traverse([&](std::string name, rhea::Component* c)->bool {
			if (rhea::Item* it = doCast<rhea::Item>(c)) {
				getManager<rhea::Item>()->Ensure(m_core.get(), it);
			}
			return true;
			});
	}

	void GraphicsService::onInit()
	{
		if (!m_platform) {
			LOG(LOG_ERROR) << "GraphicsService::onInit: platform not set";
			return;
		}
		else {
			// get backend config from application config
			std::string backend = "auto";
			rhea::ConfigFile* config = m_app->getConfig();			
			if (config)
				backend = config->getValueString("renderer.backend", "auto");
			
			m_core->init(static_cast<rhea::PlatformService*>(m_platform)->getEngine(), backend);
		}

		// create a render context for HUD / GUI
		Poseidon::Viewport* viewport = m_core->get_scene_manager()->getViewport();
		std::string hud_context_name = "HUD";
		Poseidon::RenderContext* context = viewport->createRenderContext(hud_context_name);
		context->setStencilState(1);
		// add a camera for this context
		Poseidon::Camera* cam = m_core->createCamera();
		context->setCamera(cam);
		int viewport_width = viewport->getWidth();
		int viewport_height = viewport->getHeight();
		if (!viewport_width)
			viewport_width = 800;
		if (!viewport_height)
			viewport_height = 600;
		context->setSize((float)viewport_width, (float)viewport_height);
		cam->setAspectRatio((float)viewport_width / (float)viewport_height);
		cam->setOrthographicSize((float)viewport_height);
		cam->setOrthographic(true);
		cam->setProjectionOffset(Math::Vector3(-0.5f, -0.5f, 0.0f));
		cam->updateProjection();
		cam->updateView();

		// register input handler
		m_app->getUnitTree()->registerEventHandler("scene_input", this, std::bind(&GraphicsService::onInput, this, std::placeholders::_1, std::placeholders::_2));
		m_app->getUnitTree()->registerEventHandler("scene_activate", this, [&](rhea::Event* ev, rhea::Unit* tree)->bool {
			onSceneActivate(doCast<rhea::Scene>(ev->getTarget()));
			return false;
			});
		m_app->getUnitTree()->registerEventHandler("scene_deactivate", this, [&](rhea::Event* ev, rhea::Unit* tree)->bool {
			onSceneDeactivate(doCast<rhea::Scene>(ev->getTarget()));
			return false;
			});
	}
	
	bool GraphicsService::onFrameFinal(float delta)
	{
		return m_core->iterate();
	}
	
	void GraphicsService::onSceneActivate(rhea::Scene* scene)
	{
		if (!scene) {
			LOG(LOG_ERROR) << "No scene object was provided to onSceneActivate";
			return;
		}

		m_scenes.push_back(scene);

		// try manager
		GraphicsManager* mgr = getManager(scene->getType());		
		if (mgr) {
			mgr->Ensure(m_core.get(), scene);
		}
		else {
			LOG(LOG_ERROR) << "no graphics scene manager defined.";
			return;
		}

		scene->registerEventHandler("add_item", this, std::bind(&GraphicsService::onAddItem, this, std::placeholders::_1, std::placeholders::_2));
		scene->registerEventHandler("scene_show", this, [&](rhea::Event* ev, rhea::Unit* unit)->bool {
			showScene(doCast<rhea::Scene>(ev->getTarget()));
			return false;
			});
		scene->registerEventHandler("scene_hide", this, [&](rhea::Event* ev, rhea::Unit* unit)->bool {
			hideScene(doCast<rhea::Scene>(ev->getTarget()));
			return false;
			});
	}

	void GraphicsService::onSceneDeactivate(rhea::Scene* scene)
	{
		if (!scene) {
			LOG(LOG_ERROR) << "No scene object was provided to onSceneDeactivate";
			return;
		}

		m_scenes.erase(std::remove(m_scenes.begin(), m_scenes.end(), scene));
	}

	void GraphicsService::showScene(rhea::Scene* scene)
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
		std::string render_context_name = scene->getValueString("rendercontext", "default");
		Poseidon::RenderContext* render_context = scene_mgr->getViewport()->getRenderContext(render_context_name);
		if (!render_context) {
			LOG(LOG_ERROR) << "GraphicsService::showScene: render context '" << render_context_name << "' not found";
			return;
		}
		render_context->addRenderChild((rhea::Item* )scene);

		// scene children with other contexts
		// handle all current subtree nodes
		scene->traverse([scene, scene_mgr](std::string& name, rhea::Component* unit)->bool {
			if (scene == unit)
				return true;
			rhea::Item* item = doCast<rhea::Item>(unit);
			if (!item)
				return true;
			// we simply assume if the rendercontext is set we must add a render child
			std::string render_context_name = item->getValueString("rendercontext", "");
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

	void GraphicsService::hideScene(rhea::Scene* scene)
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
		std::string render_context_name = scene->getValueString("rendercontext", "default");
		Poseidon::RenderContext* render_context = scene_mgr->getViewport()->getRenderContext(render_context_name);
		if (!render_context) {
			LOG(LOG_ERROR) << "GraphicsService::hideScene: render context '" << render_context_name << "' not found";
			return;
		}
		render_context->removeRenderChild((rhea::Item* )scene);

		// scene children with other contexts
		// handle all current subtree nodes
		scene->traverse([scene, scene_mgr](std::string& name, rhea::Component* unit)->bool {
			if (scene == unit)
				return true;
			rhea::Item* item = doCast<rhea::Item>(unit);
			if (!item)
				return true;
			// we simply assume if the rendercontext is set we must add a render child
			std::string render_context_name = item->getValueString("rendercontext", "");
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

	bool GraphicsService::onInput(rhea::Event* ev, rhea::Unit* unit)
	{
		if (ev->isEvent("input")) {
			rhea::EventInput* e = static_cast<rhea::EventInput*>(ev);
			std::shared_ptr<Psio::MouseInputEvent> me = std::static_pointer_cast<Psio::MouseInputEvent>(e->getEvent());
			int viewport_width;
			int viewport_height;
			std::string context_name;
			rhea::Unit* picked;

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
				
				std::shared_ptr<Psio::WinSizeEvent> me = std::static_pointer_cast<Psio::WinSizeEvent>(e->getEvent());
				viewport_width = me->width();
				viewport_height = me->height();
				if (viewport_width > 0 && viewport_height > 0)
				{
					Poseidon::Window* window = m_core->get_scene_manager()->getWindow(me->getWindow());
					if (!window) {
						LOG(LOG_ERROR) << "Resize event for unknown window";
						break;
					}
					// TODO handle special render targets that are not bound to this window
					window->onResize(viewport_width, viewport_height);

					// iterate over contexts and their cameras and set new values
					float aspect_ratio = (float)viewport_width / (float)viewport_height;
					for (const auto& context : window->getRenderContextList()) {
						context->setSize((float)viewport_width, (float)viewport_height);
						Poseidon::Camera* cam = context->get_camera();
						if (cam) {
							cam->setAspectRatio(aspect_ratio);
							cam->setOrthographicSize((float)viewport_height);
							cam->updateProjection();
							cam->updateView();
						}
						// send resize event to all render children - they may need to update render targets or similar
						rhea::SizeEvent size_ev(nullptr, viewport_width, viewport_height);
						for (auto& child : context->getRenderChildren()) {
							child->sendEvent("viewport_size", &size_ev);
						}
					}
					sendEvent("resize", nullptr);
				}
				
				break;
			}
		}
		return false;
	}

	rhea::Unit* GraphicsService::pick()
	{
		static Poseidon::Mesh* m_mesh = nullptr;

		// ray tracing to current hover object
		auto* mgr = doCast<rhea::PhysicsManagerService>(m_app->getService("physics_mgr"));
		if (!mgr) return nullptr;
		rhea::PhysicsService* phys = mgr->getPhysicsService();
		if (!phys) return nullptr;
		std::string def = "default";
		Poseidon::Viewport* viewport = m_core->get_scene_manager()->getViewport();
		Poseidon::Camera* camera = m_core->get_scene_manager()->getViewport()->getRenderContext(def)->get_camera();
		if (!camera) return nullptr;
			
		// compute the ray
		Math::Ray ray = camera->getPickingRay((float)m_mx, (float)m_my, viewport->getWidth(), viewport->getHeight());

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
}
