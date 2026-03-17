#ifndef RHEA_GUI_H
#define RHEA_GUI_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace Rhea
{

	class GUIFabric : public Rhea::IFabric
	{
	public:

		// fabrics functions
		void Register(Rhea::Application* app) override;

		const char* getName() const override { return "GUI"; }
	};
}

#endif // !RHEA_GUI_H
