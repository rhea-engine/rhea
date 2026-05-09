/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_DEBUGDRAWFABRIC_H
#define RHEA_DEBUGDRAWFABRIC_H

#include <Rhea/Application.h>

namespace rhea
{
	class Application;

	/** Fabrics for debug drawing
	*/
	class DebugDrawFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "DebugDraw"; }
	};
}

#endif // !RHEA_DEBUGDRAWFABRIC_H
