#ifndef RHEACLIENT_GRAPHICS_MESHGPUCACHE_H
#define RHEACLIENT_GRAPHICS_MESHGPUCACHE_H

#include <Poseidon/Core.h>
#include <Poseidon/Mesh.h>
#include <Rhea/Core/Mesh.h>

#include <functional>
#include <string>

namespace rhea::client {
    class MeshGpuCache {
    public:
        using Builder = std::function<bool(Poseidon::Core*, Rhea::Mesh* , Poseidon::Mesh*)>;

        Poseidon::Mesh* getOrCreate(Poseidon::Core* core, Rhea::Mesh* meshUnit, bool dynamic_usage, const Builder& builder);
        Poseidon::Mesh* find(Rhea::Mesh* mesh);
        Poseidon::Mesh* find(std::string key);

    private:
        std::unordered_map<std::string, std::unique_ptr<Poseidon::Mesh>> m_cache;
    };
}

#endif // !RHEACLIENT_GRAPHICS_MESHGPUCACHE_H
