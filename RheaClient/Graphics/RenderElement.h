#ifndef RHEA_GRAPHICS_RENDERELEMENT_H
#define RHEA_GRAPHICS_RENDERELEMENT_H

#include <Poseidon/Core.h>
#include <Poseidon/Node.h>
#include <Poseidon/RenderContext.h>
#include <Poseidon/Mesh.h>

#include "GraphicsElement.h"
#include <Rhea/Event.h>

#include <Poseidon/Node.h>

namespace Rhea
{
	class Unit;
	class GraphicsService;

	class GraphicsRenderElement : public GraphicsElement
	{
	public:
		GraphicsRenderElement(Poseidon::Core* core) :
			m_core(core),			
			m_entity(nullptr),
			m_mesh(nullptr),
			m_render_context(nullptr)
		{}
		~GraphicsRenderElement();

		// positioning		
		void setPosition(Rhea::Position& position);
		void setPosition(Math::Vector3 position);

		// set direction
		void lookAt(Math::Vector3 look_at);
		
		// setters
		void SetRenderEntity(Poseidon::Node* node) { m_entity = node; }
		void setRenderContext(Poseidon::RenderContext* context) { m_render_context = context; }
		void SetMesh(Poseidon::Mesh* mesh) { m_mesh = mesh; }

		// getters
		Poseidon::Node* getRenderEntity() { return m_entity; }
		Poseidon::RenderContext* getRenderContext() { return m_render_context; }
		Poseidon::Mesh* getMesh() { return m_mesh; }		

	protected:		
		Poseidon::Core* m_core;
		Poseidon::Node* m_entity;
		Poseidon::RenderContext* m_render_context;
		Poseidon::Mesh* m_mesh;
	};
}

#endif // !RHEA_GRAPHICS_RENDERELEMENT_H
