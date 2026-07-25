/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#pragma once
#include <Rhea/Service.h>
#include <functional>
#include <unordered_map>
#include <string>
#include <memory>

#include <Math/Vector3.h>
#include "PhysicsBodyDescriptor.h"
#include "PhysicsBody.h"
#include "IPhysicsDebugDraw.h"

namespace rhea
{
    class Scene;
    class Item;
    class Unit;

    class PhysicsService : public Service
    {
        MECLASS_VIRTUAL(PhysicsService, Service)
    public:
        PhysicsService(std::string name, std::string type_name)
            : Service(std::move(name), std::move(type_name)) {
        }       

        virtual void activateScene(Scene* scene) = 0;
		virtual void deactivateScene(Scene* scene) = 0;
        virtual void initializeItem(Item* item) = 0;
        virtual void removeItem(Item* item) = 0;

        virtual Unit* ray(math::Vector3 origin, math::Vector3 direction) { return nullptr; }

        struct PhysicsQueryHit
        {
            Unit* unit = nullptr;
            math::Vector3 position;
            math::Vector3 normal;
            float distance = 0.0f;
        };

        virtual bool sphereCast(const math::Vector3& from, const math::Vector3& to, float radius, PhysicsQueryHit& hit, const Unit* ignoredUnit = nullptr) = 0;

        using CreatorFunc = std::function<std::unique_ptr<PhysicsBody>(PhysicsService*, Item*)>;
        void registerPhysicsComponent(const std::string& name, CreatorFunc func) { m_physics_component_registry[name] = func; }

        virtual std::unique_ptr<PhysicsBody> createBody(const PhysicsBodyDesc& desc, Item* item) {
            return nullptr;
        }

        virtual IPhysicsDebugDraw* getDebugDrawer() { return nullptr; }

    protected:
        std::unordered_map<std::string, CreatorFunc> m_physics_component_registry;
    };
}
