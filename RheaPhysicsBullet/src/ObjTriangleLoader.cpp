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
#include <Rhea/Resources/TextResource.h>
#include <tiny_obj_loader/tiny_obj_loader.h>
#include <Rhea/Components/MeshComponent.h>
#include <Rhea/Core/Mesh.h>

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

    // 3) Mesh unit should have geometry="cube/cube_obj"
    std::string geoRef;
    std::string geoBasepathRef;
    geoRef = meshUnit->getValueString("geometry");
	geoBasepathRef = meshUnit->getBasePath();
    if (geoRef.empty()) {
        LOG(LOG_ERROR) << "ExtractTrianglesFromScene: mesh unit missing 'geometry' ref: " << mc->meshRef().id();
        return false;
    }

    // 4) Resolve geometry unit (your ini block [cube/cube_obj], type wavefront)
    rhea::Unit* geoUnit = doCast<rhea::Unit>(app->getUnitTree()->find(geoRef, geoBasepathRef, false));
    if (!geoUnit) {
        LOG(LOG_ERROR) << "ExtractTrianglesFromScene: geometry unit not found: " << geoRef;
        return false;
    }

    // 5) Geometry unit should have path="//local/cube1.obj"
    std::string objPathRef;
    objPathRef = geoUnit->getValueString("path");
    if (objPathRef.empty()) {
        LOG(LOG_ERROR) << "ExtractTrianglesFromScene: geometry unit missing 'path': " << geoRef;
        return false;
    }

    // 6) Load triangle soup from OBJ (physics-safe parser)
    rhea::TextResource* obj = (rhea::TextResource*)app->getUnitTree()->find(objPathRef, geoBasepathRef, false);
    if (!obj) {
        DLOG(LOG_DEBUG) << "Obj not found in objloader. Path was: " << (std::string)objPathRef;
        return false;
    }

    std::string objbuf;
    std::string mtlbuf;

    objbuf = obj->getAsString();

    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config;
    bool change_handedness = true;

    bool ret = reader.ParseFromString(objbuf, mtlbuf, config);

    tinyobj::attrib_t attrib = reader.GetAttrib();
    std::vector<tinyobj::shape_t>shapes = reader.GetShapes();
    std::vector<tinyobj::material_t>materials = reader.GetMaterials();

    // Copy all vertices once
    size_t vertexCount = attrib.vertices.size() / 3;
    out.positions.reserve(vertexCount);

    for (size_t v = 0; v < vertexCount; ++v) {
        float x = attrib.vertices[3 * v + 0];
        float y = attrib.vertices[3 * v + 1];
        float z = attrib.vertices[3 * v + 2];
        if (change_handedness) z = -z;
        out.positions.emplace_back(x, y, z);
    }

    // Now just push indices
    for (const auto& shape : shapes)
    {
        const auto& mesh = shape.mesh;
        size_t indexOffset = 0;

        for (size_t f = 0; f < mesh.num_face_vertices.size(); f++)
        {
            int fv = mesh.num_face_vertices[f];

            for (int k = 1; k < fv - 1; ++k)
            {
                out.indices.push_back(mesh.indices[indexOffset + 0].vertex_index);
                out.indices.push_back(mesh.indices[indexOffset + k].vertex_index);
                out.indices.push_back(mesh.indices[indexOffset + k + 1].vertex_index);
            }

            indexOffset += fv;
        }
    }
   
    if (out.indices.empty() || out.positions.empty()) {
        LOG(LOG_ERROR) << "ExtractTrianglesFromScene: empty triangle soup for " << objPathRef;
        return false;
    }

    return true;
}
