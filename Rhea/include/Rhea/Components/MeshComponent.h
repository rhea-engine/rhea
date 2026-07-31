/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_MESHCOMPONENT_H
#define RHEA_MESHCOMPONENT_H

#include <Rhea/Component.h>
#include <Rhea/Ref.h>
#include <string>

namespace rhea
{
    class Mesh;
    
    /** Manages logical mesh asset - both reference and resolved instance */
    class MeshComponent : public Component
    {
        RECLASS(MeshComponent, Component, "//meworla/meshcomponent")
        REPROPERTY(Ref<Mesh>, "meshRef", Label("Mesh Reference"), Edit, Read);
    public:
        MeshComponent(std::string name, std::string type_name) :
            Component(name, type_name),
            m_mesh(nullptr)
        {}
        void ready() override {
			if (m_values.find("meshRef") != m_values.end())
                m_meshRef = Ref<Mesh>(m_values["meshRef"].toString(), m_attributes["basepath"]);
			if (m_parent && !m_parent->getComponent<MeshComponent>()) {
				m_parent->addComponent(this);
			}
        }

        std::vector<ResourceReference> getReferences() override
        {
            auto it = m_values.find("meshRef");

            if (it == m_values.end())
                return {};

            return { {
                "meshRef",
                "//meworla/mesh",
                it->second,
                ResourceUsage::Load
            } };
        }

        // Asset reference (unresolved)
        const Ref<Mesh>& meshRef() const { return m_meshRef; }
        void setMeshRef(Ref<Mesh> ref) { 
            m_meshRef = std::move(ref);
        }

        // Resolved asset
        Mesh* getMesh() const { return m_mesh; }
        void setMesh(Mesh* m) { m_mesh = m; }

    private:
        // Unresolved reference
        Ref<Mesh> m_meshRef;
        
        // Resolved instance
        Mesh* m_mesh = nullptr;
    };
}
#endif