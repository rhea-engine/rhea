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

        virtual void setScene(Scene* scene) = 0;
        virtual void initializeItem(Item* item) = 0;
        virtual void removeItem(Item* item) = 0;

        void onInit() = 0;

        virtual void toggleDebug() {}
        virtual Unit* ray(math::Vector3 origin, math::Vector3 direction) { return nullptr; }

        using CreatorFunc = std::function<std::unique_ptr<PhysicsBody>(PhysicsService*, Item*)>;
        void registerPhysicsComponent(const std::string& name, CreatorFunc func) { m_physics_component_registry[name] = func; }

        virtual std::unique_ptr<PhysicsBody> createBody(const PhysicsBodyDesc& desc, Item* item) {
            return nullptr;
        }

        virtual IPhysicsDebugDraw* debugDraw() { return nullptr; }

    protected:
        std::unordered_map<std::string, CreatorFunc> m_physics_component_registry;
    };
}
