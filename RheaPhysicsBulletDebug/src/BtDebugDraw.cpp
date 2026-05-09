/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <RheaPhysicsBulletDebug/BtDebugDraw.h>
#include <bullet/src/btBulletDynamicsCommon.h>
#include <Rhea/Application.h>
#include <Poseidon/Mesh.h>
#include <Poseidon/RenderQueue.h>
#include <Poseidon/FrameRenderer.h>
#include <Hippios/VertexBuffer.h>
#include <Poseidon/Shader.h>
#include <Poseidon/RenderContext.h>
#include <Poseidon/Material.h>
#include <Rhea/Components/MeshComponent.h>
#include <Rhea/DebugDraw/LineBatchMesh.h>
#include <Psio/Log.h>
#include <RheaPhysicsBullet/RheaPhysicsBulletService.h>

namespace rhea::physics::bullet::debug
{
	struct BulletDebugDraw::Impl
	{
		class DebugDraw : public btIDebugDraw {

		public:
			DebugDraw() :
				m_normals(false),
				m_mode(0)
			{
			}

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

		Impl(Application* a, btDynamicsWorld* w, Item* owner)
			: m_app(a),
			m_world(w),
			m_owner(owner)
		{
			auto* vis = owner->addComponentUnique<rhea::VisualComponent>("visual");
			rhea::MeshComponent* ownerLineMeshComp = owner->addComponentUnique<rhea::MeshComponent>("mesh");
			ownerLineMesh = simpleparser::Type::create<rhea::LineBatchMesh>("cube_line_mesh");
			ownerLineMesh->setup(m_app, {}, {});
			ownerLineMesh->ready();
			ownerLineMesh->update();
			ownerLineMeshComp->setMesh(ownerLineMesh);

			/*
			m_mesh = new poseidon::Mesh();

			m_mesh->setViewport(core->getFrameRenderer()->getViewport());			
			m_num_faces = 0;
			m_enabled = true;

			// prepare shaders		
			material = core->getMaterial("color");
			if (!material) {
				material = core->createMaterial();
				material->setTopology(2);
				poseidon::Shader* shader = core->getShader("shader_vp");
				if (!shader) {
					shader = core->createShader("shader_vp");					
					if (!shader->loadFromFile("Data\\Rhea\\shaders\\shader_vp.hlsl", "VShader", "PShader"))
						exit(-1);
				}
				material->setShader(shader);
				core->registerMaterial("color", material);
			}

			core->getFrameRenderer()->getViewport()->getRenderContext(std::string("default"))->addRenderChild(m_owner);
			*/
			int mode = DebugDraw::DBG_DrawText |
				DebugDraw::DBG_DrawFeaturesText |
				DebugDraw::DBG_DrawContactPoints;
			mode |= btIDebugDraw::DBG_DrawWireframe /* + btIDebugDraw::DBG_DrawConstraints + btIDebugDraw::DBG_DrawConstraintLimits*/;
			mode |= DebugDraw::DBG_DrawAabb;
			mode |= DebugDraw::DBG_DrawNormals;
			m_drawer.setDebugMode(mode);

			m_world->setDebugDrawer(&m_drawer);
		}

		~Impl()
		{
			if (m_world && m_world->getDebugDrawer() == &m_drawer)
				m_world->setDebugDrawer(nullptr);
		}

		Application* m_app = nullptr;
		btDynamicsWorld* m_world = nullptr;
		Item* m_owner;

		DebugDraw m_drawer;
		bool m_enabled = true;

		rhea::LineBatchMesh* ownerLineMesh = nullptr;

		// Debug geometry buffers
		std::vector<Line> lines;
		std::vector<Triangle> triangles;

		// Helpers
		void clearFrameData()
		{
			lines.clear();
			triangles.clear();
		}	
	};

	BulletDebugDraw::BulletDebugDraw(std::string name, std::string type_name) :
		m_impl(nullptr),
		IPhysicsDebugDraw(name, type_name)
	{
	}

	void BulletDebugDraw::setup(rhea::Application* app, rhea::PhysicsService* physics_service)
	{
		Item::setup(app, {}, {});
		if (doCast<BulletPhysicsService>(physics_service) == nullptr) {
			LOG(LOG_ERROR) << "BulletDebugDraw requires a BulletPhysicsService.";
			return;
		}
		m_impl = new Impl(app, doCast<BulletPhysicsService>(physics_service)->getWorld(), this);
	}

	BulletDebugDraw::~BulletDebugDraw()
	{
		delete m_impl;
		m_impl = nullptr;
	}

	void BulletDebugDraw::onFrame(float delta)
	{
		if (getComponent<VisualComponent>()->isVisible() == false)
			return;

		// Collect debug geometry data
		m_impl->m_drawer.m_lines.clear();
		m_impl->m_drawer.m_triangles.clear();

		static int nums = 0;

		m_impl->m_world->debugDrawWorld();

		//if (nums == m_drawer.m_lines.size())
		//	return;

		nums = (int)m_impl->m_drawer.m_lines.size();

		// draw to the mesh
		m_impl->ownerLineMesh->clearLines();

		// Render lines		
		float fact = 1;
		for (auto lit = m_impl->m_drawer.m_lines.begin(); lit != m_impl->m_drawer.m_lines.end(); lit++) {
			Line& line = *lit;	
			math::Vector4 color = math::Vector4(line.color.x, line.color.y, line.color.z, 1.0f) * fact;
			m_impl->ownerLineMesh->addLine(line.p0, line.p1, color);
		}
		m_impl->ownerLineMesh->update();

		// render triangles
		/*
		std::vector<hippios::vertex> t_vertices;
		std::vector<unsigned long> t_indices;
		for (auto t_it = m_drawer.m_triangles.begin(); t_it != m_drawer.m_triangles.end(); t_it++) {
			const Triangle& tri = *t_it;
			t_indices.push_back(t_vertices.size());
			t_vertices.push_back({ tri.p0.x, tri.p0.y, tri.p0.z, tri.color.x, tri.color.y, tri.color.z, 1.0 });
			t_indices.push_back(t_vertices.size());
			t_vertices.push_back({ tri.p1.x, tri.p1.y, tri.p1.z, tri.color.x, tri.color.y, tri.color.z, 1.0 });
			t_indices.push_back(t_vertices.size());
			t_vertices.push_back({ tri.p2.x, tri.p2.y, tri.p2.z, tri.color.x, tri.color.y, tri.color.z, 1.0 });
		}
		if (t_vertices.size() && m_num_faces) {
			m_mesh->setVertices(sizeof(hippios::vertex), t_vertices.size(), t_vertices.data());
			m_num_faces = t_indices.size() / 3;
			m_mesh->setIndices(sizeof(unsigned long), m_num_faces, t_indices.data());
		}
		*/

	}

	void BulletDebugDraw::toggle()
	{
		auto* m_vis = getComponent<VisualComponent>();
		m_vis->setVisible(!m_vis->isVisible());
		m_impl->m_enabled = m_vis->isVisible();
		Event e("update", this);
		sendEvent("update", &e);
	}

	void BulletDebugDraw::setEnabled(bool on)
	{
		auto* m_vis = getComponent<VisualComponent>();
		m_vis->setVisible(on);
		m_impl->m_enabled = m_vis->isVisible();
		Event e("update", this);
		sendEvent("update", &e);
	}

	bool BulletDebugDraw::isEnabled() const
	{
		return m_impl->m_enabled;
	}

	void BulletDebugDraw::Impl::DebugDraw::setDebugMode(int mode)
	{
		m_mode = mode;
	}

	int BulletDebugDraw::Impl::DebugDraw::getDebugMode() const
	{
		return m_mode;
	}

	void BulletDebugDraw::Impl::DebugDraw::reportErrorWarning(const char* warning)
	{
		LOG(LOG_DEBUG) << "btDbgWarn: " << warning;
	}

	void BulletDebugDraw::Impl::DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		float r = color.getX();
		float g = color.getY();
		float b = color.getZ();

		//printf("(%f,%f,%f)/(%f,%f,%f)\n", from.x(), from.y(), from.z(), to.x(), to.y(), to.z());

		// Hack to get rid of triangle normals.  The hack is based on the assumption
		// that only normals are drawn in yellow.
		if (m_normals == false && r == 1.0f && g == 1.0f && b == 0.0f) return;

		Line line;

		line.p0 = math::Vector3((float)from.getX(),
			(float)from.getY(),
			(float)from.getZ());
		line.p1 = math::Vector3((float)to.getX(),
			(float)to.getY(),
			(float)to.getZ());
		line.color = math::Vector3((float)r,
			(float)g,
			(float)b);

		m_lines.push_back(line);
	}

	void BulletDebugDraw::Impl::DebugDraw::drawContactPoint(const btVector3& point, const btVector3& normal, btScalar distance, int lifetime, const btVector3& color)
	{
		const btVector3 to = point + normal * distance;
		const btVector3& from = point;

		drawLine(from, to, color);
	}
	void BulletDebugDraw::Impl::DebugDraw::draw3dText(const btVector3& location, const char* text)
	{
		printf("draw3dText not implemented: %s\n", text);
	}

	void BulletDebugDraw::Impl::DebugDraw::drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& color, btScalar)
	{
		btScalar r = color.getX();
		btScalar g = color.getY();
		btScalar b = color.getZ();

		Triangle tri;

		tri.p0 = math::Vector3((float)v0.getX(),
			(float)v0.getY(),
			(float)v0.getZ());

		tri.p1 = math::Vector3((float)v1.getX(),
			(float)v1.getY(),
			(float)v1.getZ());

		tri.p2 = math::Vector3((float)v2.getX(),
			(float)v2.getY(),
			(float)v2.getZ());

		tri.color = math::Vector3((float)r,
			(float)g,
			(float)b);

		m_triangles.push_back(tri);
	}

	void BulletDebugDraw::Impl::DebugDraw::drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& n0, const btVector3& n1, const btVector3& n2, const btVector3& color, btScalar alpha)
	{
		printf("drawTriangle(2) - not yet implemented!\n");	
	}

	void BulletDebugDraw::Impl::DebugDraw::drawSphere(btScalar radius, const btTransform& transform, const btVector3& color)
	{
		btVector3 center = transform.getOrigin();

		const btVector3 xoffs = transform.getBasis() * btVector3(1, 0, 0);
		const btVector3 yoffs = transform.getBasis() * btVector3(0, 1, 0);
		const btVector3 zoffs = transform.getBasis() * btVector3(0, 0, 1);

		drawArc(center, xoffs, yoffs, radius, radius, 0, SIMD_2_PI, color, false, 10.0);
		drawArc(center, yoffs, zoffs, radius, radius, 0, SIMD_2_PI, color, false, 10.0);
		drawArc(center, zoffs, xoffs, radius, radius, 0, SIMD_2_PI, color, false, 10.0);
	}
}