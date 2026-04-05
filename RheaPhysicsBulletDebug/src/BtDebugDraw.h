#ifndef RHEA_PHYSICS_BULLET_DEBUGDRAW_H
#define RHEA_PHYSICS_BULLET_DEBUGDRAW_H

#include <vector>
#include <Math/Vector3.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Physics/IPhysicsDebugDraw.h>

namespace poseidon { class RenderQueue; class Camera; class Material; class Texture; class Mesh; class Core; }
namespace hippios { class VertexBuffer; class IndexBuffer; }
class btDynamicsWorld;

namespace rhea
{
    class Application;
}

namespace rhea::physics::bullet::debug
{
	class BulletDebugDraw : public rhea::Item, public rhea::IPhysicsDebugDraw
	{      
    public:
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
        
        BulletDebugDraw(rhea::Application* app, btDynamicsWorld* world, poseidon::Core* core);
		~BulletDebugDraw();

        //  Renderable functions
        void cullPhase(poseidon::RenderQueue* queue, poseidon::Camera* camera);
        void getRenderInput(hippios::VertexBuffer** pvb, hippios::IndexBuffer** pib, int& num_indices);
        poseidon::Material* getMaterial();
        poseidon::Texture* getTexture();
        std::vector<poseidon::Texture*>& getTextures();

        void toggle() override;        

        void setEnabled(bool on) override;     // toggle
        bool isEnabled() const override;

    protected:
        struct Impl;
		Impl* m_impl;
	};
}

#endif // ! RHEA_PHYSICS_BULLET_DEBUGDRAW_H
