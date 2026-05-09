/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICS_BULLET_DEBUGDRAW_H
#define RHEA_PHYSICS_BULLET_DEBUGDRAW_H

#include <vector>
#include <Math/Vector3.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Physics/IPhysicsDebugDraw.h>
#include <Rhea/Physics/PhysicsService.h>
#include <RheaPhysicsBullet/RheaPhysicsBulletService.h>

namespace poseidon { class RenderQueue; class Camera; class Material; class Texture; class Mesh; class Core; }
namespace hippios { class VertexBuffer; class IndexBuffer; }
class btDynamicsWorld;

namespace rhea
{
    class Application;
}

namespace rhea::physics::bullet::debug
{
	class BulletDebugDraw : public rhea::IPhysicsDebugDraw
	{      
		RECLASS(BulletDebugDraw, IPhysicsDebugDraw, "//meworla/bulletdebugdraw")
    public:
        BulletDebugDraw(std::string name, std::string type_name);
        struct Line {
            math::Vector3 p0;
            math::Vector3 p1;
            math::Vector3 color;
        };

        struct Triangle {
            math::Vector3 p0;
            math::Vector3 p1;
            math::Vector3 p2;
            math::Vector3 color;
        };
        
        void setup(rhea::Application* app, rhea::PhysicsService* physics_service);
		~BulletDebugDraw();

        //  Renderable functions
		void onFrame(float delta) override;
 

        void toggle() override;        

        void setEnabled(bool on) override;     // toggle
        bool isEnabled() const override;

    protected:
        struct Impl;
		Impl* m_impl;
	};
}

#endif // ! RHEA_PHYSICS_BULLET_DEBUGDRAW_H
