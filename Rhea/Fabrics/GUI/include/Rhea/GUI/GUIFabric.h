/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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
