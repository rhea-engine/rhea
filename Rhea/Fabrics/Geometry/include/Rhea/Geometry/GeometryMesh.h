/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GEOMETRYMESH_H
#define RHEA_GEOMETRYMESH_H

#include <Rhea/Core/Mesh.h>

namespace rhea
{
    class GeometryMesh : public Mesh {
        RECLASS_VIRTUAL(GeometryMesh, Mesh, "//meworla/geometrymesh")
    public:
        GeometryMesh(std::string name, std::string type_name) :
            Mesh(name, type_name)
        {}
        GeometryMesh(std::string name) :
            GeometryMesh(name, "//meworla/geometrymesh")
        {}
		
		virtual void createMesh() = 0;
        void rebuildMesh() {
			if (!m_dirty)
				return;
			m_dirty = false;
            createMesh();
		}

        void markDirty() { m_dirty = true; }
    private:
        bool m_dirty = true;
    };
}

#endif // !RHEA_GEOMETRYMESH_H
