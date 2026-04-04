#ifndef RHEA_GUI_H
#define RHEA_GUI_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea
{

	class GUIFabric : public rhea::IFabric
	{
	public:

		// fabrics functions
		void Register(rhea::Application* app) override;

		const char* getName() const override { return "GUI"; }
	};
}

#endif // !RHEA_GUI_H
