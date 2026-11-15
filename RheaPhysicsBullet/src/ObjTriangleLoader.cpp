/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "ObjTriangleLoader.h"

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Components/MeshComponent.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/ObjFile/ObjDocument.h>

bool ExtractTrianglesFromScene(rhea::Application* app, rhea::Item* item, TriangleSoup& out)
{
    out.positions.clear();
    out.indices.clear();

    if (!app || !item) {
        LOG(LOG_ERROR) << "ExtractTrianglesFromScene: null app/item";
        return false;
    }

    // get mesh ref
    // 1) Get mesh reference from MeshComponent (preferred)
    auto* mc = item->getComponent<rhea::MeshComponent>();
    if (!mc) {
        LOG(LOG_ERROR) << "ExtractTrianglesFromScene: item has no MeshComponent";
        return false;
    }
    
    if (mc->meshRef().empty()) {
        LOG(LOG_ERROR) << "ExtractTrianglesFromScene: item has no mesh ref (MeshComponent.meshRef is empty)";
        return false;
    }

    // 2) Resolve mesh unit
    rhea::Mesh* meshUnit = app->getUnitTree()->resolve(mc->meshRef());
    if (!meshUnit) {
        LOG(LOG_ERROR) << "ExtractTrianglesFromScene: mesh unit not found: " << mc->meshRef().id();
        return false;
    }

	// 3) Resolve geometry reference from mesh unit
    auto* geometry = app->getUnitTree()->resolve(meshUnit->geometryRef());
    auto* obj_document = doCast<rhea::ObjDocument>(geometry);
    if (!obj_document) {
        LOG(LOG_ERROR) << "ObjTriangleLoader: geometry is not an OBJ document: " << meshUnit->geometryRef().id();
        return false;
    }

	// 4) Resolve OBJ resource from ObjDocument
    const auto& obj_ref = obj_document->objectRef();
    auto* obj = app->getUnitTree()->resolve(obj_ref);

    if (!obj) {
        LOG(LOG_ERROR) << "ObjTriangleLoader: OBJ resource not found: " << obj_ref.id();
        return false;
    }

    return obj_document->extractTriangles(out.positions, out.indices);
}
