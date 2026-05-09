/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GEOMETRYFABRIC_H
#define RHEA_GEOMETRYFABRIC_H

namespace rhea
{
	class Application;

	/** Fabrics for geometries
	*/
	class GeometryFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Geometry"; }
	};
}

#endif // !RHEA_GEOMETRYFABRIC_H
