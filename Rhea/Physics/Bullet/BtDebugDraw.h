#ifndef RHEA_PHYSICS_BULLET_DEBUGDRAW_H
#define RHEA_PHYSICS_BULLET_DEBUGDRAW_H

#include <vector>
#include <Math/Vector3.h>
#include <Rhea/Core/Item.h>

namespace Poseidon { class RenderQueue; class Camera; class Material; class Texture; class Mesh; class Core; }
namespace Hippios { class VertexBuffer; class IndexBuffer; }
class btDynamicsWorld;

namespace Rhea
{
    class Application;

	class BulletDebugDraw : public Item
	{
    public:
        struct Line {
            Math::Vector3 p0;
            Math::Vector3 p1;
            Math::Vector3 color;
        };

        struct Triangle {
            Math::Vector3 p0;
            Math::Vector3 p1;
            Math::Vector3 p2;
            Math::Vector3 color;
        };

        BulletDebugDraw(Application* app, btDynamicsWorld* world, Poseidon::Core* core);
		~BulletDebugDraw();

        //  Renderable functions
        void cullPhase(Poseidon::RenderQueue* queue, Poseidon::Camera* camera);
        void getRenderInput(Hippios::VertexBuffer** pvb, Hippios::IndexBuffer** pib, int& num_indices);
        Poseidon::Material* getMaterial();
        Poseidon::Texture* getTexture();
        std::vector<Poseidon::Texture*>& getTextures();

        void toggle();        

    protected:
        struct Impl;
		Impl* m_impl;
	};
}

#endif // ! RHEA_PHYSICS_BULLET_DEBUGDRAW_H
