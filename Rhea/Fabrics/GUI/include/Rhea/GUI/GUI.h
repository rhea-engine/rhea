#ifndef RHEA_GUI_H
#define RHEA_GUI_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Rhea
{

	class GUIModule : public Rhea::IFabricModule
	{
	public:

		// fabrics functions
		void Register(Rhea::Application* app) override;

		const char* getName() const override { return "GUI"; }
	};
}

#endif // !RHEA_GUI_H
