/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <RheaClient/Graphics/GraphicsService.h>
#include <functional>
#include <cstring>

#include <Rhea/Platform/PlatformService.h>
#include <Rhea/Application.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Scene.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Graphics/SizeEvent.h>

#include "Poseidon/Core.h"
#include "Poseidon/Viewport.h"
#include "Poseidon/FrameRenderer.h"
#include "Poseidon/RenderContext.h"
#include "Poseidon/Camera.h"
#include "Poseidon/Window.h"
#include "Poseidon/RenderWorld.h"

#include <Rhea/Element.h>
#include <RheaClient/Graphics/Texture.h>
#include "ItemManager.h"
#include "SceneManager.h"
#include <RheaClient/Graphics/MaterialManager.h>
#include <RheaClient/Graphics/CameraManager.h>
#include "MeshManager.h"

#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/EventInput.h>

#include <Rhea/Core/Item.h>
#include <Rhea/Core/Mesh.h>
#include "Poseidon/Renderable.h"

#include <Rhea/Core/Material.h>
#include <Rhea/Core/Camera.h>

#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/ConfigFile.h>
#include <RheaClient/Window/WindowService.h>
#include "LightManager.h"
#include <RheaClient/Components/GraphicsComponent.h>
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/Resources/ResourceService.h>
#include <Poseidon/Texture.h>
#include <Rhea/Core/SceneResource.h>

namespace rhea::client {
	GraphicsService::GraphicsService(std::string name, std::string type_name) :
		m_platform(nullptr),
		m_mx(0),
		m_my(0),
		m_fullscreen(false),
		m_main_window(nullptr),
		rhea::IGraphicsService(name, type_name)
	{
		m_core = std::make_unique<poseidon::Core>();

		addManager<rhea::Scene>(std::make_unique<GraphicsSceneManager>(this));
		addManager<rhea::Light>(std::make_unique<GraphicsLightManager>(this));
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

	GraphicsManager* GraphicsService::getManager(simpleparser::Type* type)
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
		auto* mgr = getManager(unit->getType());
		if (!mgr && doCast<rhea::Item>(unit))
			mgr = getManager<rhea::Item>();
		if (mgr) {
			mgr->Ensure(m_core.get(), unit);
			return true;
		}
		return false;
	}

	void GraphicsService::setPlatform(rhea::Service* platform_service)
	{
		m_platform = platform_service;
	}

	bool GraphicsService::onAddItem(rhea::Event* event, rhea::Unit* sender)
	{
		auto* item = doCast<rhea::Item>(event->getTarget());
		auto* scene = doCast<rhea::Scene>(sender);

		if (!item)
			return false;

		if (!addElement(item)) {
			LOG(LOG_ERROR) << "Could not prepare graphics for " << item->getPath();
			return false;
		}

		poseidon::RenderContext* context = nullptr;

		std::string own_context = item->getValueString("rendercontext", "");

		if (!own_context.empty()) {
			context = m_main_window->getDefaultViewport()->getRenderContext(own_context);
		}
		else if (auto* parent_item = doCast<rhea::Item>(item->getParent())) {
			if (auto* parent_rc = parent_item->getComponent<RenderComponent>()) {
				context = parent_rc->ctx;
			}
		}
		else if (scene) {
			std::string scene_context = scene->getValueString("rendercontext", "default");
			context = m_main_window->getDefaultViewport()->getRenderContext(scene_context);
		}

		if (!context) {
			LOG(LOG_ERROR) << "No render context for item " << item->getPath();
			return false;
		}

		registerItemToContext(item, context);
		return false;
	}

	bool GraphicsService::onAddItemRecursive(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Unit* root = doCast<rhea::Unit>(ev->getTarget());	
		if (!root) return false;

		root->traverse([&](std::string& name, rhea::Component* c)->bool {

			if (rhea::Item* item = doCast<rhea::Item>(c)) {
				if (!addElement(item)) {
					LOG(LOG_ERROR) << "Could not prepare graphics for " << item->getPath();
					return false;
				}

				// add to render context if parented to scene
				rhea::Scene* scene = doCast<rhea::Scene>(item->getParent());
				if (scene) {
					std::string render_context_name = scene->getValueString("rendercontext", "default");
					if (!m_main_window) {
						LOG(LOG_ERROR) << "GraphicsService::showScene: main window not initialized";
						return false;
					}
					poseidon::RenderContext* render_context = m_main_window->getDefaultViewport()->getRenderContext(render_context_name);
					if (!render_context) {
						LOG(LOG_ERROR) << "GraphicsService::showScene: render context '" << render_context_name << "' not found";
						return false;
					}
					// Check if item has specific render context
					std::string item_ctx_name = item->getValueString("rendercontext", "");
					poseidon::RenderContext* target_ctx = render_context;

					if (!item_ctx_name.empty()) {
						target_ctx = m_main_window->getDefaultViewport()->getRenderContext(item_ctx_name);
						if (!target_ctx) {
							LOG(LOG_ERROR) << "GraphicsService::showScene: render context '"
								<< item_ctx_name << "' not found for item '"
								<< item->getPathName() << "'";
							return true; // Continue with other items
						}
					}

					// Add item to appropriate context
					registerItemToContext(item, target_ctx);
				}
				else {
					// item is not parented to a scene - get context of parent item if available
					rhea::Item* parent_item = doCast<rhea::Item>(item->getParent());
					if (parent_item) {
						auto* parent_rc = parent_item->getComponent<rhea::client::RenderComponent>();
						if (parent_rc && parent_rc->ctx) {
							registerItemToContext(item, parent_rc->ctx);
						}
					}
				}

				return true;
			}

			// we do not create assets here - could be added if we want eager creation

			return true;
			});

		return false;

	}

	void GraphicsService::addItem(std::string render_context, rhea::Item* item)
	{
		if (!item) {
			LOG(LOG_ERROR) << "GraphicsService::addItem: item is null";
			return;
		}

		if (!m_main_window) {
			LOG(LOG_ERROR) << "GraphicsService::addItem: main window not initialized";
			return;
		}
		poseidon::Viewport* viewport = m_main_window->getDefaultViewport();
		if (!viewport) {
			LOG(LOG_ERROR) << "GraphicsService::addItem: no viewport available";
			return;
		}

		poseidon::RenderContext* context = viewport->getRenderContext(render_context);
		if (!context) {
			LOG(LOG_ERROR) << "GraphicsService::addItem: render context '" << render_context << "' not found";
			return;
		}

		// Create instance elements for all items in subtree
		item->traverse([&](std::string name, rhea::Component* c)->bool {
			if (rhea::Item* child_item = doCast<rhea::Item>(c)) {
				getManager<rhea::Item>()->Ensure(m_core.get(), child_item);
			}
			return true;
			});

		registerItemToContextRecursive(item, context);
	}

	bool GraphicsService::onRemoveItem(rhea::Event* event, rhea::Unit*)
	{
		auto* item = doCast<rhea::Item>(event->getTarget());

		if (!item)
			return false;

		auto* rc = item->getComponent<RenderComponent>();

		if (rc)
			unregisterItemFromContext(item, rc->ctx);
		else
			unregisterItemFromContext(item, nullptr);

		return false;
	}

	bool GraphicsService::onRemoveItemRecursive(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Unit* root = doCast<rhea::Unit>(ev->getTarget());
		if (!root) return false;

		root->traverse([&](std::string& name, rhea::Component* c)->bool {
			if (rhea::Item* item = doCast<rhea::Item>(c)) {
				//getManager<rhea::Item>()->Release(m_core.get(), item);
				// remove from render context if set
				auto* rc = item->getComponent<RenderComponent>();
				if (rc && rc->ctx) {
					unregisterItemFromContext(item, rc->ctx);
				}
				return true;
			}
			return true;
			});
		return false;
	}

	void GraphicsService::onInit()
	{
		if (!m_platform) {
			LOG(LOG_ERROR) << "GraphicsService::onInit: platform not set";
			return;
		}

		// get backend config from application config
		std::string backend = "auto";
		rhea::ConfigFile* config = m_app->getConfig();
		if (config)
			backend = config->getValueString("renderer.backend", "auto");

		m_core->init(static_cast<rhea::PlatformService*>(m_platform)->getEngine(), backend);
		WindowRegistryService* window_registry = doCast<WindowRegistryService>(m_app->getService("window_registry"));
		for (auto window : m_app->getWindows()) {
			poseidon::Window* win = m_core->createWindow(window.title, window.width, window.height, window.name);
			if (win) {
				
				if (window_registry) {
					window_registry->registerWindow(win, window.name);
				}
			}
			if (!m_main_window) {
				m_main_window = win;
			}
		}

		// For OpenGL we need to call this after window creation, for D3D12 it can be called before -
		// but it needs to be called after backend initialization and before any render context creation
		m_core->initializeRenderStates();

		if (!m_main_window) {
			LOG(LOG_ERROR) << "GraphicsService::onInit: no window created";
			return;
		}

		// create the render contexts for all windows
		if (window_registry) {
			for (auto window : m_app->getWindows()) {
				poseidon::Window* win = window_registry->getRenderWindowByName(window.name);
				if (!win)
					continue;

				// iterate over the windows viewports and create render contexts for its layers
				poseidon::Viewport* viewport = nullptr;
				for (auto& vp : window.views) {
					if (vp.name == "default") {
						viewport = win->getDefaultViewport();
					}
					else {
						viewport = win->createViewport(vp.name, vp.x, vp.y, vp.width, vp.height);
					}
					for (auto& layer : vp.layers) {
						// create context for viewport if not exists
						poseidon::RenderContext* context = viewport->getRenderContext(layer.name);
						if (!context) {
							context = viewport->createRenderContext(layer.name);
							if (layer.name == "HUD") {
								context->setRenderOrderMode(poseidon::RenderContext::RenderOrderMode::TreeOrdered);
								context->setStencilState(1);

								// add a camera for this context

								int viewport_width = viewport->getWidth();
								int viewport_height = viewport->getHeight();
								if (!viewport_width)
									viewport_width = 800;
								if (!viewport_height)
									viewport_height = 600;

								poseidon::CameraHandle cam_handle = m_core->getRenderWorld()->createCamera();
								auto* gfx_cam_mgr = static_cast<rhea::client::GraphicsCameraManager*>(getManager<rhea::Camera>());
								if (gfx_cam_mgr) {
									gfx_cam_mgr->syncCameraProjection(m_core.get(), cam_handle, nullptr, (float)viewport_width, (float)viewport_height);
								}
								context->setCameraHandle(cam_handle);
								context->setSize((float)viewport_width, (float)viewport_height);
								// since we have no rhea Camera for this camera, we need to set orthographic mode and projection offset manually
								poseidon::Camera* cam = m_core->getRenderWorld()->resolveCamera(cam_handle);
								cam->setOrthographic(true);
								cam->setProjectionOffset(math::Vector3(-0.5f, -0.5f, 0.0f));
							}
						}
					}
				}
			}
		}		

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

		// load core resources
		rhea::ResourceList res_list(m_app, {});

		// load shaders
		res_list.push("standard_shader", "//rhea/shaders/standard.shader", "//meworla/shader");
		res_list.push("widget_shader", "//rhea/shaders/widget.shader", "//meworla/shader");
		res_list.push("text_shader", "//rhea/shaders/text.shader", "//meworla/shader");
		res_list.push("terrain_shader", "//rhea/shaders/terrain.shader", "//meworla/shader");
		res_list.push("skeletal_shader", "//rhea/shaders/skeletal.shader", "//meworla/shader");
		res_list.push("lines_shader", "//rhea/shaders/lines.shader", "//meworla/shader");

		// skybox		
		hippios::DepthStencilStateDesc dss{};
		dss.depth_enable = true;
		dss.depth_write_mask = hippios::DepthWriteMask::DEPTH_WRITE_ALL;
		dss.depth_func = hippios::ComparisonFunction::COMPARE_LESS_EQUAL;
		doCast<GraphicsService>(m_app->getService("graphics"))->getEngine()->get_renderer()->createStencilState("skybox", dss);
		res_list.push("skybox_shader", "//rhea/shaders/skybox.shader", "//meworla/shader");

		// note: for locally installed resources this is actually synchronous
		res_list.load([this](rhea::Error* err){
			if (err) {
				LOG(LOG_ERROR) << "Error loading core shader resources: " << err->getMessage();
			}
			else {
				LOG(LOG_INFO) << "Core shader resources loaded successfully.";
			}
		});
		// instantiate all shaders
		Unit* parent = doCast<Unit>(m_app->getUnitTree()->find("//rhea/shaders", true));
		for (auto& [key, res] : res_list.getAllComponents()) {
			SceneResource* shader_res = doCast<SceneResource>(res.resource);
			if (shader_res) {
				shader_res->spawn(parent, [](ReqError err, Unit* unit){
					if (err != ReqError::OK) {
						LOG(LOG_DEBUG) << "Shader resource could not be spawned: " << unit->getPathName() << " (error: " << static_cast<int>(err) << ")";
					}
					});
			}
		}

		// create a white fallback texture for missing textures
		m_white_fallback_texture.reset(m_core->createTexture(0, 1)); // TEXTURE_2D, 1 image
		if (m_white_fallback_texture) {
			unsigned char white_pixel[4] = { 255, 255, 255, 255 }; // RGBA white
			m_white_fallback_texture->load(0, 1, 1, white_pixel);
		}
	}

	bool GraphicsService::onFrameFinal(float delta)
	{
		// update bridges
		auto* item_mgr = getManager<rhea::Item>();
		if (item_mgr) {
			item_mgr->onFrame(delta);
		}
		m_core->getFrameRenderer()->advanceTime(delta);
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
		if (!hasEventHandler("scene_item_added", this))
			scene->registerEventHandler("scene_item_added", this, std::bind(&GraphicsService::onAddItem, this, std::placeholders::_1, std::placeholders::_2));
		if (!hasEventHandler("scene_item_removed", this))
			scene->registerEventHandler("scene_item_removed", this, std::bind(&GraphicsService::onRemoveItem, this, std::placeholders::_1, std::placeholders::_2));
		if (!hasEventHandler("scene_show", this))
			scene->registerEventHandler("scene_show", this, [&](rhea::Event* ev, rhea::Unit* unit)->bool {
			showScene(doCast<rhea::Scene>(ev->getTarget()));
			return false;
				});
		if (!hasEventHandler("scene_hide", this))
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

		m_scenes.erase(std::remove(m_scenes.begin(), m_scenes.end(), scene), m_scenes.end());
	}

	void GraphicsService::showScene(rhea::Scene* scene)
	{
		if (!scene) {
			LOG(LOG_ERROR) << "No scene object was provided to showScene";
			return;
		}

		poseidon::FrameRenderer* frame_renderer = m_core->getFrameRenderer();
		if (!frame_renderer) {
			LOG(LOG_ERROR) << "GraphicsService::showScene: no scene manager";
			return;
		}

		std::string render_context_name = scene->getValueString("rendercontext", "default");
		if (!m_main_window) {
			LOG(LOG_ERROR) << "GraphicsService::showScene: main window not initialized";
			return;
		}
		poseidon::RenderContext* render_context = m_main_window->getDefaultViewport()->getRenderContext(render_context_name);
		if (!render_context) {
			LOG(LOG_ERROR) << "GraphicsService::showScene: render context '" << render_context_name << "' not found";
			return;
		}

		// Add all scene items to the render context
		scene->children([&](std::string& name, rhea::Component* c)->bool {
			rhea::Item* item = doCast<rhea::Item>(c);
			if (!item)
				return true;

			// Add item to appropriate context
			registerItemToContextRecursive(item, render_context);

			return true;
			}, true); // add ordered
	}

	void GraphicsService::registerItemToContextRecursive(rhea::Item* item, poseidon::RenderContext* render_context)
	{
		if (!item || !render_context) {
			LOG(LOG_ERROR) << "GraphicsService::registerItemToContextRecursive: item or context is null";
			return;
		}

		// Check if item has specific render context
		std::string item_ctx_name = item->getValueString("rendercontext", "");
		poseidon::RenderContext* target_ctx = render_context;

		if (!item_ctx_name.empty()) {
			target_ctx = m_main_window->getDefaultViewport()->getRenderContext(item_ctx_name);
			if (!target_ctx) {
				LOG(LOG_ERROR) << "GraphicsService::registerItemToContextRecursive: render context '"
					<< item_ctx_name << "' not found for item '"
					<< item->getPathName() << "'";
				return; // ignore this item but continue with children - they may have their own render context
			}
		}

		registerItemToContext(item, target_ctx);
		item->children([&](std::string name, rhea::Component* c)->bool {
			rhea::Item* child = doCast<rhea::Item>(c);
			if (child) {
				registerItemToContextRecursive(child, target_ctx);
				return true;
			}
			else {
				// not an item - try children
				return true;
			}
			}, true);
	}
	void GraphicsService::hideScene(rhea::Scene* scene)
	{
		if (!scene) {
			LOG(LOG_ERROR) << "No scene object was provided to hideScene";
			return;
		}

		for (auto* item : scene->getItems()) {
			auto* rc = item->getComponent<rhea::client::RenderComponent>();
			if (rc && rc->ctx) {
				unregisterItemFromContext(item, rc->ctx);
			}
		}
	}

	bool GraphicsService::onInput(rhea::Event* ev, rhea::Unit* unit)
	{
		if (ev->isEvent("input")) {
			rhea::EventInput* e = static_cast<rhea::EventInput*>(ev);
			InputEvent me = e->getEvent();
			int viewport_width;
			int viewport_height;
			std::string context_name;
			rhea::Unit* picked;

			switch (me.type) {
			case InputEventType::MouseMoved:
				m_mx = me.x;
				m_my = me.y;
				break;
			case InputEventType::MouseButtonPressed:
				picked = pick();
				if (picked)
					picked->sendEvent("scene_input", ev);
				break;

			case InputEventType::WindowResized:
				// resize window

				viewport_width = me.width;
				viewport_height = me.height;
				if (viewport_width > 0 && viewport_height > 0)
				{
					WindowRegistryService* window_registry = doCast<WindowRegistryService>(m_app->getService("window_registry"));
					poseidon::Window* window = nullptr;
					if (window_registry) {
						window = window_registry->getRenderWindow(me.window);
					}
					if (!window) {
						LOG(LOG_ERROR) << "Resize event for unknown window";
						break;
					}
					// TODO handle special render targets that are not bound to this window
					window->onResize(viewport_width, viewport_height);

					// iterate over all viewports and find those bound to this window, then update their contexts and cameras
					for (auto& vp : window->getViewports()) {
						// iterate over contexts and their cameras and set new values
						float aspect_ratio = (float)viewport_width / (float)viewport_height;
						for (const auto& contextPtr : vp->getRenderContextList()) {
							poseidon::RenderContext* context = contextPtr.get();
							context->setSize((float)viewport_width, (float)viewport_height);
							auto* gfxCamMgr = static_cast<rhea::client::GraphicsCameraManager*>(getManager<rhea::Camera>());
							if (gfxCamMgr && context->getCameraHandle() != poseidon::InvalidCamera) {
								gfxCamMgr->syncCameraProjection(getEngine(), context->getCameraHandle(), nullptr, (float)viewport_width, (float)viewport_height);
							}
							// send resize event to all items of this context - they may need to update render targets or similar
							auto it = m_context_items.find(context);
							if (it != m_context_items.end()) {
								const std::vector<rhea::Item*> snapshot = it->second;
								for (auto& item : snapshot) {
									auto current_it = m_context_items.find(context);
									if (current_it == m_context_items.end())
										break;

									const auto& current_items = current_it->second;
									if (std::find(
										current_items.begin(),
										current_items.end(),
										item) == current_items.end()) {
										continue;
									}
									rhea::SizeEvent size_ev(item, viewport_width, viewport_height);
									item->sendEvent("size", &size_ev);
								}
							}
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
		static poseidon::Mesh* m_mesh = nullptr;

		// ray tracing to current hover object
		auto* mgr = doCast<rhea::PhysicsManagerService>(m_app->getService("physics_mgr"));
		if (!mgr) return nullptr;
		rhea::PhysicsService* phys = mgr->getPhysicsService();
		if (!phys) return nullptr;
		std::string def = "default";
		poseidon::Viewport* viewport = m_core->getFrameRenderer()->getViewport();
		poseidon::Camera* camera = m_core->getFrameRenderer()->getViewport()->getRenderContext(def)->getCameraHandle() != poseidon::InvalidCamera ?
			m_core->getRenderWorld()->resolveCamera(m_core->getFrameRenderer()->getViewport()->getRenderContext(def)->getCameraHandle()) : nullptr;
		if (!camera) return nullptr;

		// compute the ray
		math::Ray ray = camera->getPickingRay((float)m_mx, (float)m_my, viewport->getWidth(), viewport->getHeight());

		math::Vector3 ray_from = ray.getOrigin();
		math::Vector3 ray_dir = ray.getDirection();
		ray_dir *= 1000.0f;
		math::Vector3 ray_to = ray.getOrigin() + ray_dir;

		// call ray test from physics service
		return phys->ray(ray_from, ray_to);

#ifdef PICK3D_DEBUG
		if (!m_mesh) {
			// prepare debug item

			m_mesh = new poseidon::Mesh();
			m_mesh->setViewport(m_app->get_core()->getFrameRenderer()->getViewport());
			poseidon::Entity* entity = m_core->createEntity(m_mesh);
			entity->setName("ray_test");
			poseidon::Material* material = m_app->get_core()->getMaterial("color");
			entity->setMaterial(material);
			m_app->get_core()->get_scene_manager()->getViewport()->getRenderContext(std::string("default"))->addRenderChild(entity);
		}

		std::vector<hippios::vertex> l_vertices;
		math::Vector3 pos = ray.getOrigin();
		math::Vector3 dir = ray_to;

		//dir *= 1000.0f;

		l_vertices.push_back({ pos.x, pos.y, pos.z, 0.0f, 1.0f, 0.0f, 1.0f });
		l_vertices.push_back({ dir.x, dir.y, dir.z, 0.0f, 1.0f, 0.0f, 1.0f });
		m_mesh->setVertices(sizeof(hippios::vertex), l_vertices.size(), l_vertices.data());

		DLOG(LOG_DEBUG) << "pos: " << std::to_string(pos.x) << "/" + std::to_string(pos.y) << "/" << std::to_string(pos.z);
		DLOG(LOG_DEBUG) << "ray: " << std::to_string(dir.x) << "/" + std::to_string(dir.y) << "/" + std::to_string(dir.z);

		math::Vector3 deb1 = camera->getProjectionMatrixRS() * camera->getViewMatrix() * pos;
		math::Vector3 deb2 = camera->getProjectionMatrixRS() * camera->getViewMatrix() * dir;

		DLOG(LOG_DEBUG) << "deb1: " << std::to_string(deb1.x) << "/" + std::to_string(deb1.y) << "/" + std::to_string(deb1.z);
		DLOG(LOG_DEBUG) << "deb2: " << std::to_string(deb2.x) << "/" + std::to_string(deb2.y) << "/" + std::to_string(deb2.z);
#endif
	}

	void GraphicsService::registerItemToContext(rhea::Item* item, poseidon::RenderContext* context)
	{
		if (!item || !context) return;

		poseidon::RenderNodeId parent_id = poseidon::InvalidRenderNode;
		rhea::Item* parent_item = doCast<rhea::Item>(item->getParent());
		if (parent_item) {
			auto* parent_rc = parent_item->getComponent<rhea::client::RenderComponent>();
			if (parent_rc && parent_rc->gfx_node_id != poseidon::InvalidGraphicsNode) {
				parent_id = context->findNodeForSource(parent_rc->gfx_node_id);
			}
			else {
				auto* parent_gc = parent_item->getComponent<rhea::client::GraphicsComponent>();
				if (parent_gc && parent_gc->gfx_node_id != poseidon::InvalidGraphicsNode) {
					parent_id = context->findNodeForSource(parent_gc->gfx_node_id);
				}
			}
		}
		poseidon::RenderNodeId render_node_id = poseidon::InvalidRenderNode;
		auto* rc = item->getComponent<rhea::client::RenderComponent>();
		if (rc) {
			render_node_id = context->registerOrderNode(rc->gfx_node_id, parent_id);

			if (rc->ctx && rc->ctx != context) {
				LOG(LOG_WARNING) << "Silently moving item between render contexts: " <<
					item->getPath() <<
					". Use unregisterItemFromContext() first to avoid this warning.";
				unregisterItemFromContext(item, rc->ctx);
			}
			rc->ctx = context;

			if (rc->hasRenderable())
				context->attachRenderable(render_node_id, rc->renderable);
			if (rc->hasLight())
				context->addLight(rc->light);

			// Track items in this context - avoid duplicates
			auto& items = m_context_items[context];
			if (std::find(items.begin(), items.end(), item) == items.end()) {
				items.push_back(item);
			}

			// Ensure() selects a camera only when its renderer representation is
			// created for the first time. When a scene is shown again, the camera
			// already exists, so explicitly restore it as the active camera of the
			// scene's render context.
			if (doCast<rhea::Camera>(item) && rc->hasCamera()) {
				context->setCameraHandle(rc->camera);
			}
			//LOG(LOG_DEBUG) << "Registered item '" << item->getPath() << "' to render context '" << context->getName() << "', rc-node id: " << rc->gfx_node_id << ", rendernode id: " << render_node_id << ", parent id: " << parent_id;
		}
		else {
			auto gc = item->getComponent<rhea::client::GraphicsComponent>();
			if (!gc) {
				item->addComponentUnique<rhea::client::GraphicsComponent>("graphics", rhea::client::GraphicsManager::allocateNextId());
				gc = item->getComponent<rhea::client::GraphicsComponent>();
			}
			render_node_id = context->registerOrderNode(gc->gfx_node_id, parent_id);
			gc->ctx = context;
			//LOG(LOG_DEBUG) << "Registered item '" << item->getPath() << "' to render context '" << context->getName() << "', gc-node id: " << gc->gfx_node_id << ", rendernode id: " << render_node_id << ", parent id: " << parent_id;
		}
		if (render_node_id == poseidon::InvalidRenderNode) {
			// failed
			return;
		}

		// set visibility
		auto* vis = item->getComponent<rhea::VisualComponent>();
		if (vis) {
			context->setVisibility(render_node_id, vis->isVisible());
		}

		// send size event
		hippios::ViewportDesc* viewport_size = context->getSize();
		rhea::SizeEvent size_ev(item, viewport_size->w, viewport_size->h);
		item->sendEvent("size", &size_ev);
	}

	void GraphicsService::unregisterItemFromContext(rhea::Item* item, poseidon::RenderContext* context)
	{
		if (!item) abort();

		auto* rc = item->getComponent<rhea::client::RenderComponent>();
		// Detach renderable from context
		if (rc && context && rc->ctx == context) {
			
				poseidon::RenderNodeId node_id = context->findNodeForSource(rc->gfx_node_id);
				if (node_id != poseidon::InvalidRenderNode) {

					if (rc->hasRenderable())
						context->detachRenderable(node_id);

					if (rc->hasLight()) {
						context->removeLight(rc->light);
					}
					context->unregisterOrderNode(node_id);
				}

				rc->ctx = nullptr;			
		}
		else {
			// safety check if in any other contexts
			for (auto& [ctx, items] : m_context_items) {
				auto it = std::find(items.begin(), items.end(), item);
				if (it != items.end()) {
					items.erase(it);
					if (rc)
						LOG(LOG_WARNING) << "GraphicsService: removed deleted item from other context '" << ctx->getName() << "', "
						<< (rc->ctx ? " (rc->ctx: " + rc->ctx->getName() + ")" : "rc->ctx was null");
					else
						LOG(LOG_WARNING) << "GraphicsService: removed deleted item from context '" << ctx->getName() << "' without render component";
				}
			}
		}

		// Remove from registry
		if (context) {
			auto it = m_context_items.find(context);
			if (it != m_context_items.end()) {
				auto& items = it->second;
				items.erase(std::remove(items.begin(), items.end(), item), items.end());

				// Clean up empty entries
				if (items.empty()) {
					m_context_items.erase(it);
				}
			}
			else
				LOG(LOG_DEBUG) << "GraphicsService::unregisterItemFromContext: context '" << context->getName() << "' not found in registry";
		}
	}

	void GraphicsService::dumpStats() const
	{
		LOG(LOG_INFO) << "GraphicsService Stats:";
		LOG(LOG_INFO) << "Scenes: " << m_scenes.size();
		for (auto* scene : m_scenes) {
			LOG(LOG_INFO) << "Scene '" << scene->getPath() << "' items: " << scene->getItems().size();
		}
		LOG(LOG_INFO) << "Render contexts: " << m_context_items.size();
		for (const auto& pair : m_context_items) {
			const auto* context = pair.first;
			const auto& items = pair.second;
			LOG(LOG_INFO) << "Context '" << context->getName() << "' has " << items.size() << " items.";
		}
	}
}
