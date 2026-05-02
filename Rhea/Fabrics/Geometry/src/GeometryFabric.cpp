/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Application.h>

#include <Rhea/Geometry/GeometryFabric.h>
#include <Rhea/Geometry/CubeMesh.h>
#include <Rhea/Geometry/PlaneMesh.h>
#include <Rhea/Geometry/SphereMesh.h>
#include <Rhea/Geometry/CylinderMesh.h>
#include <Rhea/Geometry/ConeMesh.h>

namespace rhea
{
	void GeometryFabric::Register(Application* app)
	{
		CubeMesh::_register_CubeMesh();
		PlaneMesh::_register_PlaneMesh();
		SphereMesh::_register_SphereMesh();
		CylinderMesh::_register_CylinderMesh();
		ConeMesh::_register_ConeMesh();
	}
}