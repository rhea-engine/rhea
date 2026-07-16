/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/Scene.h"

#include <memory>

#include "Rhea/Core/Item.h"
#include "Rhea/Core/SceneLoader.h"
#include "Rhea/Event.h"
#include "Rhea/Application.h"
#include "Rhea/UnitTree.h"
#include <Psio/Log.h>

namespace rhea
{
	void Scene::onParented()
	{		
		LOG(LOG_NOTICE) << "scene ready";
		ready(); 
	}

	void Scene::onRemoved()
	{
		if (m_is_shown)
			hide();

		if (m_is_active)
			deactivate();

		unregisterEventHandler("scene_enter", this);
		unregisterEventHandler("attached_item", this);
		unregisterEventHandler("detached_item", this);
	}

	void Scene::addItem(Item* item)
	{
		if (!item->getParent() || item->getParent() == this) {
			addChild(item);
		}
	}	

	void Scene::removeItem(Item* item)
	{
		removeChild(item);
	}

	void Scene::ready()
	{
		Unit::ready();

		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Item* child = doCast<Item>(it->second);
			if (child)
				m_items.push_back(child);
		}
		registerEventHandler("scene_enter", this, std::bind(&Scene::onSceneEnter, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("attached_item", this, std::bind(&Scene::onItemAttached, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("detached_item", this, std::bind(&Scene::onItemDetached, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool Scene::onSceneEnter(Event* ev, Unit* unit)
	{
		// we capture this event
		ev->stopPropagation();
		// send add item
		Event e("scene_item_added", ev->getTarget());
		sendEvent("scene_item_added", &e);
		return true;
	}

	bool Scene::onItemAttached(Event* ev, Unit* unit)
	{
		// we capture this event
		ev->stopPropagation();

		Item* item = doCast<Item>(ev->getTarget());
		if (!item) {
			LOG(LOG_ERROR) << "Scene::onItemAttached: attached item is not of type Item";
			return true;
		}

		item->traverse([&](std::string&, Component* c) {
			if (auto* child_item = doCast<Item>(c)) {				
					m_items.push_back(child_item);
					Event ev("scene_item_added", child_item);
					sendEvent("scene_item_added", &ev);
			}
			if (m_is_active) {
				c->onActivate();
			}
			return true;
			});		
		return false;
	}

	bool Scene::onItemDetached(Event* ev, Unit* unit)
	{
		// we capture this event
		ev->stopPropagation();

		Item* item = doCast<Item>(ev->getTarget());
		if (!item) {
			LOG(LOG_ERROR) << "Scene::onItemDetached: detached item is not of type Item";
			return true;
		}

		item->traverse([&](std::string&, Component* c) {
			if (auto* childItem = doCast<Item>(c)) {
				if (m_is_active) {
					c->onDeactivate();
				}
				m_items.erase(
					std::remove(m_items.begin(), m_items.end(), childItem),
					m_items.end()
				);
				Event e("scene_item_removed", item);
				sendEvent("scene_item_removed", &e);
			}
			return true;
			});
		return false;
	}

	void Scene::show()
	{
		if (!m_app) {
			LOG(LOG_ERROR) << "Scene: No app pointer set, did you forget to call setup?";
			return;
		}

		// the scene needs to be active
		if (!m_is_active)
			activate();

		if (!m_is_shown) {
			m_is_shown = true;

			rhea::Event ev("scene_show", this);
			sendEventChildren(std::string("scene_show"), &ev);
			broadcast("show");
			m_app->onScene(this);
		}
	}

	void Scene::hide()
	{
		if (m_is_shown) {
			m_is_shown = false;
			rhea::Event ev("scene_hide", this);
			sendEventChildren("scene_hide", &ev);
		}
	}

	void Scene::activate()
	{
		if (!m_is_active) {
			traverse([&](std::string&, Component* c) {
				c->onActivate();
				return true;
				});
			Event e("scene_activate", this);
			m_app->getUnitTree()->sendEvent("scene_activate", &e);
			m_is_active = true;			
		}
	}

	void Scene::deactivate()
	{
		if (m_is_active) {
			if (m_is_shown)
				hide();

			Event e("scene_deactivate", this);
			m_app->getUnitTree()->sendEvent("scene_deactivate", &e);
			traverse([&](std::string&, Component* c) {
				c->onDeactivate();
				return true;
				});
			m_is_active = false;
		}
	}
}