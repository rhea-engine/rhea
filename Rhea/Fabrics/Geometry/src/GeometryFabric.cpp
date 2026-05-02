/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Application.h>

#include <Rhea/Geometry/GeometryFabric.h>
#include <Rhea/Geometry/CubeMesh.h>

namespace rhea
{
	void GeometryFabric::Register(Application* app)
	{
		CubeMesh::_register_CubeMesh();
	}
}