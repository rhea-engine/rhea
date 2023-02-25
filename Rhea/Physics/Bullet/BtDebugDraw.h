#ifndef RHEA_PHYSICS_BULLET_DEBUGDRAW_H
#define RHEA_PHYSICS_BULLET_DEBUGDRAW_H

#include <vector>

#include "btBulletDynamicsCommon.h"
#include <Math/Vector3.h>
#include <Poseidon/Renderable.h>
#include <Poseidon/Mesh.h>
#include <vector>
#include <Poseidon/Material.h>
#include <Rhea/Fabrics/3D/Item.h>

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

        BulletDebugDraw(Application* app, btDynamicsWorld* world);

        //  Renderable functions
        void cullPhase(Poseidon::RenderQueue* queue, Poseidon::Camera* camera);
        void getRenderInput(Hippios::VertexBuffer** pvb, Hippios::IndexBuffer** pib, int& num_indices);
        Poseidon::Material* getMaterial();
        Poseidon::Texture* getTexture();
        std::vector<Poseidon::Texture*>& getTextures() { return m_textures; }

        void toggle();

        class DebugDraw : public btIDebugDraw {

        public:
            DebugDraw() :
                m_normals(false),
                m_mode(0)
            {}

            virtual void setDebugMode(int mode);
            virtual int getDebugMode() const;
            virtual void reportErrorWarning(const char* warning);

            virtual void drawLine(const btVector3& from, const btVector3& to,
                const btVector3& color);
            virtual void drawContactPoint(const btVector3& point,
                const btVector3& normal, btScalar distance, int lifetime,
                const btVector3& color);
            virtual void draw3dText(const btVector3& location, const char* text);
            virtual void drawTriangle(const btVector3& v0, const btVector3& v1,
                const btVector3& v2, const btVector3& color, btScalar);
            virtual void drawTriangle(const btVector3& v0, const btVector3& v1,
                const btVector3& v2, const btVector3& n0, const btVector3& n1,
                const btVector3& n2, const btVector3& color, btScalar alpha);
            virtual void drawSphere(btScalar radius, const btTransform& transform,
                const btVector3& color);

        public:
            std::vector<Triangle> m_triangles;
            std::vector<Line> m_lines;
            bool m_normals;
            int m_mode;
        };

    protected:
        Application* m_app;
        DebugDraw m_drawer;
        Poseidon::Mesh* m_mesh;
        btDynamicsWorld* m_world;
        int m_num_faces;
        Poseidon::Material* material;
        std::vector<Poseidon::Texture*> m_textures;
        bool m_enabled;
	};
}

#endif // ! RHEA_PHYSICS_BULLET_DEBUGDRAW_H
