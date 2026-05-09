/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Application.h>

#include <Rhea/DebugDraw/DebugDrawFabric.h>
#include <Rhea/DebugDraw/LineBatchMesh.h>

namespace rhea
{
	void DebugDrawFabric::Register(Application* app)
	{
		LineBatchMesh::_register_LineBatchMesh();
	}
}