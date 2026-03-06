#ifndef RHEA_MESHCOMPONENT_H
#define RHEA_MESHCOMPONENT_H

#include <Rhea/Component.h>
#include <string>

namespace Rhea
{
    class Mesh;
    
    /** Manages logical mesh asset - both reference and resolved instance */
    class MeshComponent : public Component
    {
        RECLASS(MeshComponent, Component, "//meworla/meshcomponent")
    public:
        MeshComponent(std::string name, std::string type_name) :
            Component(name, type_name),
            m_mesh(nullptr)
        {}

        // Asset reference (unresolved)
        const std::string& meshRef() const { return m_meshRef; }
        const std::string& basepathRef() const { return m_basePath; }
        void setMeshRef(std::string ref, std::string basepath) { 
            m_meshRef = std::move(ref); 
            m_basePath = std::move(basepath); 
        }

        // Resolved asset
        Mesh* getMesh() const { return m_mesh; }
        void setMesh(Mesh* m) { m_mesh = m; }

    private:
        // Unresolved reference
        std::string m_meshRef;
        std::string m_basePath;
        
        // Resolved instance
        Mesh* m_mesh = nullptr;
    };
}
#endif