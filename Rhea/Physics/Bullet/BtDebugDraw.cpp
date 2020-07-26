#include "BtDebugDraw.h"
#include "Application.h"
#include <Poseidon/RenderQueue.h>
#include <Poseidon/SceneManager.h>
#include <Hippios/VertexBuffer.h>
#include <Poseidon/Shader.h>
#include <Poseidon/RenderContext.h>

namespace Rhea
{
	BulletDebugDraw::BulletDebugDraw(Application* app, btDynamicsWorld* world)
	{
		m_app = app;
		m_mesh = new Poseidon::Mesh();
		m_mesh->setViewport(app->get_core()->get_scene_manager()->getViewport());
		m_world = world;
		m_num_faces = 0;	
		m_enabled = true;

		// prepare shaders		
		material = app->get_core()->getMaterial("color");
		if (!material) {
			material = app->get_core()->createMaterial();
			material->setTopology(2);
			Poseidon::Shader* shader = app->get_core()->getShader("shader_vp");
			if (!shader) {
				shader = app->get_core()->createShader("shader_vp");
				shader->setVertexType(0);
				if (shader->loadFromFile("Data\\Rhea\\shaders\\shader_vp.hlsl", "VShader", "PShader"))
					exit(-1);
			}
			material->setShader(shader);
			app->get_core()->registerMaterial("color", material);
		}

		app->get_core()->get_scene_manager()->getViewport()->getRenderContext(std::string("default"))->addRenderChild(this);
		int mode = DebugDraw::DBG_DrawText |
			DebugDraw::DBG_DrawFeaturesText |
			DebugDraw::DBG_DrawContactPoints;
		mode |= btIDebugDraw::DBG_DrawWireframe /* + btIDebugDraw::DBG_DrawConstraints + btIDebugDraw::DBG_DrawConstraintLimits*/;
		mode |= DebugDraw::DBG_DrawAabb;
		mode |= DebugDraw::DBG_DrawNormals;
		m_drawer.setDebugMode(mode);
		world->setDebugDrawer(&m_drawer);
	}

	void BulletDebugDraw::cullPhase(Poseidon::RenderQueue* queue, Poseidon::Camera* camera)
	{
		if (m_enabled)
			queue->addRenderable(this);
		else
			return;

		// Collect debug geometry data
		m_drawer.m_lines.clear();
		m_drawer.m_triangles.clear();

		static int nums = 0;

		m_world->debugDrawWorld();

		//if (nums == m_drawer.m_lines.size())
		//	return;

		nums = m_drawer.m_lines.size();

		// draw to the mesh

		// Render lines		
		std::vector<Hippios::vertex> l_vertices;
		float fact = 1;
		for (auto lit = m_drawer.m_lines.begin(); lit != m_drawer.m_lines.end(); lit++) {
			Line& line = *lit;	
			//line.p0.normalise();
			//line.p1.normalise();
			/*line.color.x = 1.0f;
			line.color.y = 1.0f;
			line.color.z = 1.0f;*/
			l_vertices.push_back({ line.p0.x, line.p0.y, line.p0.z, line.color.x, line.color.y, line.color.z, 1.0f });
			l_vertices.push_back({ line.p1.x, line.p1.y, line.p1.z, line.color.x, line.color.y, line.color.z, 1.0f });
		}

		m_mesh->setVertices(sizeof(Hippios::vertex), l_vertices.size(), l_vertices.data());
		m_num_faces = l_vertices.size();

		// render triangles
		/*
		std::vector<Hippios::vertex> t_vertices;
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
			m_mesh->setVertices(sizeof(Hippios::vertex), t_vertices.size(), t_vertices.data());
			m_num_faces = t_indices.size() / 3;
			m_mesh->setIndices(sizeof(unsigned long), m_num_faces, t_indices.data());
		}
		*/

	}

	void BulletDebugDraw::getRenderInput(Hippios::VertexBuffer** pvb, Hippios::IndexBuffer** pib, int& num_indices)
	{
		*pvb = m_mesh->get_vbuffer();
		//*pib = m_mesh->get_ibuffer();
		//num_indices = m_num_faces * 3;
		*pib = nullptr;
		num_indices = m_num_faces;
	}

	Poseidon::Material* BulletDebugDraw::getMaterial()
	{
		return material;
	}

	Poseidon::Texture* BulletDebugDraw::getTexture()
	{
		return nullptr;
	}

	void BulletDebugDraw::toggle()
	{
		m_enabled = !m_enabled;
	}

	void BulletDebugDraw::DebugDraw::setDebugMode(int mode)
	{
		m_mode = mode;
	}

	int BulletDebugDraw::DebugDraw::getDebugMode() const
	{
		return m_mode;
	}

	void BulletDebugDraw::DebugDraw::reportErrorWarning(const char* warning)
	{
		printf("btDbgWarn: %s\n", warning);
	}

	void BulletDebugDraw::DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		float r = color.getX();
		float g = color.getY();
		float b = color.getZ();

		//printf("(%f,%f,%f)/(%f,%f,%f)\n", from.x(), from.y(), from.z(), to.x(), to.y(), to.z());

		// Hack to get rid of triangle normals.  The hack is based on the assumption
		// that only normals are drawn in yellow.
		if (m_normals == false && r == 1.0f && g == 1.0f && b == 0.0f) return;

		Line line;

		line.p0 = Poseidon::Vector3((float)from.getX(),
			(float)from.getY(),
			(float)from.getZ());
		line.p1 = Poseidon::Vector3((float)to.getX(),
			(float)to.getY(),
			(float)to.getZ());
		line.color = Poseidon::Vector3((float)r,
			(float)g,
			(float)b);

		m_lines.push_back(line);
	}

	void BulletDebugDraw::DebugDraw::drawContactPoint(const btVector3& point, const btVector3& normal, btScalar distance, int lifetime, const btVector3& color)
	{
		const btVector3 to = point + normal * distance;
		const btVector3& from = point;

		drawLine(from, to, color);
	}
	void BulletDebugDraw::DebugDraw::draw3dText(const btVector3& location, const char* text)
	{
		printf("draw3dText not implemented: %s\n", text);
	}

	void BulletDebugDraw::DebugDraw::drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& color, btScalar)
	{
		btScalar r = color.getX();
		btScalar g = color.getY();
		btScalar b = color.getZ();

		Triangle tri;

		tri.p0 = Poseidon::Vector3((float)v0.getX(),
			(float)v0.getY(),
			(float)v0.getZ());

		tri.p1 = Poseidon::Vector3((float)v1.getX(),
			(float)v1.getY(),
			(float)v1.getZ());

		tri.p2 = Poseidon::Vector3((float)v2.getX(),
			(float)v2.getY(),
			(float)v2.getZ());

		tri.color = Poseidon::Vector3((float)r,
			(float)g,
			(float)b);

		m_triangles.push_back(tri);
	}

	void BulletDebugDraw::DebugDraw::drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& n0, const btVector3& n1, const btVector3& n2, const btVector3& color, btScalar alpha)
	{
		printf("drawTriangle(2) - not yet implemented!\n");	
	}

	void BulletDebugDraw::DebugDraw::drawSphere(btScalar radius, const btTransform& transform, const btVector3& color)
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