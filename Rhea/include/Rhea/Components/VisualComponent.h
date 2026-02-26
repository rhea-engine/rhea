#ifndef RHEA_VISUALCOMPONENT_H
#define RHEA_VISUALCOMPONENT_H

#include <Rhea/Component.h>

namespace Rhea
{
	class VisualComponent : public Component
	{
		RECLASS(VisualComponent, Component, "//meworla/visualcomponent")
	public:
		VisualComponent(std::string name, std::string type_name) :
			m_hidden(false),			
			Component(name, type_name)
		{
		}
		const std::string& meshRef() const { return m_meshRef; }
		const std::string& basepathRef() const { return m_basePath; }
		void setMeshRef(std::string ref, std::string basepath) { m_meshRef = std::move(ref); m_basePath = std::move(basepath); }

		// visibility
		void hide() { m_hidden = true; }
		void show() { m_hidden = false; }
		bool isVisible() { return !m_hidden; }		

	protected:
		bool m_hidden;
		std::string m_meshRef;
		std::string m_basePath;
	};
}

#endif // !RHEA_VISUALCOMPONENT_H
