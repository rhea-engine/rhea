#include <Rhea/DebugDraw/LineBatchMesh.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Core/Shader.h>
#include <Psio/Log.h>
#include <Math/Matrix3.h>
#include <Math/Matrix4.h>

namespace rhea
{
	void LineBatchMesh::ready()
	{
		Mesh::ready();
		setDynamic(true);
		setIndexed(false);

		// if material is set explicitly we use it
		if (m_values.find("material") != m_values.end()) {
			m_material = doCast<Material>(m_app->getUnitTree()->find(m_values["material"].toString(), m_attributes["basepath"], false));
		}

		if (!m_material) {
			// otherwise we use the default line material
			m_material = doCast<Material>(m_app->getUnitTree()->find("//meworla/line_material", m_attributes["basepath"], false));
		}

		if (!m_material) {
			m_material = new Material(m_path + "_material");
			addChild(m_material);
			m_material->setup(m_app, {}, {});
			m_material->setValue("shader", getValueString("shadername", "lines"));

			// get the shader				
			//Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(m_values["shader"].toString(), m_attributes["basepath"], false));
			Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find("//rhea/shaders/lines", "", false));
			if (shader_res) {
				m_material->setShader(shader_res);
			}
			else {
				LOG(LOG_ERROR) << "Could not find shader resource: " << m_values["shader"].toString();
			}
		}
	}

	void LineBatchMesh::addLine(math::Vector3 a, math::Vector3 b, math::Vector4 color)
	{
		m_vertices.push_back({ a, color });
		m_vertices.push_back({ b, color });

		markDirty();
	}

	void LineBatchMesh::addCube(math::Vector3 min, math::Vector3 max, math::Vector4 color)
	{
		// add 12 lines for the cube edges
		addLine({ min.x, min.y, min.z }, { max.x, min.y, min.z }, color);
		addLine({ max.x, min.y, min.z }, { max.x, max.y, min.z }, color);
		addLine({ max.x, max.y, min.z }, { min.x, max.y, min.z }, color);
		addLine({ min.x, max.y, min.z }, { min.x, min.y, min.z }, color);
		addLine({ min.x, min.y, max.z }, { max.x, min.y, max.z }, color);
		addLine({ max.x, min.y, max.z }, { max.x, max.y, max.z }, color);
		addLine({ max.x, max.y, max.z }, { min.x, max.y, max.z }, color);
		addLine({ min.x, max.y, max.z }, { min.x, min.y, max.z }, color);
		addLine({ min.x, min.y, min.z }, { min.x, min.y, max.z }, color);
		addLine({ max.x, min.y, min.z }, { max.x, min.y, max.z }, color);
		addLine({ max.x, max.y, min.z }, { max.x, max.y, max.z }, color);
		addLine({ min.x, max.y, min.z }, { min.x, max.y, max.z }, color);
	}

	void LineBatchMesh::addCubeCentered(math::Vector3 center, math::Vector3 size, math::Vector4 color)
	{
		math::Vector3 half_size = size * 0.5f;
		addCube(center - half_size, center + half_size, color);
	}

	void LineBatchMesh::addAxis(const math::Vector3& origin, float size)
	{
		addLine(origin, origin + math::Vector3{ size, 0, 0 }, { 1, 0, 0, 1 }); // X
		addLine(origin, origin + math::Vector3{ 0, size, 0 }, { 0, 1, 0, 1 }); // Y
		addLine(origin, origin + math::Vector3{ 0, 0, size }, { 0, 1, 1, 1 }); // Z	
	}

	void LineBatchMesh::addAxis(const math::Matrix4& transform, float size)
	{
		math::Vector3 origin = transform.getTranslation();
		math::Vector3 x = transform.getXAxis();
		math::Vector3 y = transform.getYAxis();
		math::Vector3 z = transform.getZAxis();

		addLine(origin, origin + x * size, { 1, 0, 0, 1 });
		addLine(origin, origin + y * size, { 0, 1, 0, 1 });
		addLine(origin, origin + z * size, { 0, 1, 1, 1 });
	}

	void LineBatchMesh::addGrid(const math::Matrix4& transform, float size, int divisions, math::Vector4 color)
	{
		math::Vector3 origin = transform.getTranslation();
		math::Vector3 x = transform.getXAxis();
		math::Vector3 z = transform.getZAxis();
		float half_size = size * 0.5f;
		float step = size / divisions;
		for (int i = 0; i <= divisions; i++) {
			float offset = -half_size + i * step;
			addLine(origin + x * offset - z * half_size, origin + x * offset + z * half_size, color);
			addLine(origin - x * half_size + z * offset, origin + x * half_size + z * offset, color);
		}
	}

	void LineBatchMesh::clearLines()
	{
		m_vertices.clear();

		markDirty();
	}

	void LineBatchMesh::update()
	{
		if (!m_dirty) return;

		if (m_vertices.empty()) return;

		// update vertex buffer
		VertexBufferColor* vb = getVertexBufferAs<VertexBufferColor>();
		if (vb) {
			vb->setVertexSize(sizeof(VertexBufferColor::VertexColor));
			vb->updateVertices(m_vertices.data(), m_vertices.size());

			Event ev("update", this);
			sendEvent("update", &ev);		
		}
		m_dirty = false;
	}
}