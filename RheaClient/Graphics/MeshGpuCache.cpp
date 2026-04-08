#include <RheaClient/Graphics/MeshGpuCache.h>
#include <Poseidon/SceneManager.h>

namespace rhea::client
{


    poseidon::Mesh* MeshGpuCache::getOrCreate(poseidon::Core* core, rhea::Mesh* meshUnit, bool dynamic_usage, const Builder& builder)
    {
        auto key = meshUnit->getPath(); // or stable asset id
        auto it = m_cache.find(key);
        if (it != m_cache.end()) return it->second.get();

        auto poseMesh = std::make_unique<poseidon::Mesh>(dynamic_usage);
        poseMesh->setViewport(core->get_scene_manager()->getViewport());

        if (builder) {
            if (!builder(core, meshUnit, poseMesh.get())) {
                // builder failed -> don't cache a broken mesh
                return nullptr;
            }
        }

        auto* ptr = poseMesh.get();
        m_cache.emplace(key, std::move(poseMesh));
        return ptr;
    }

    poseidon::Mesh* MeshGpuCache::find(rhea::Mesh* mesh)
    {
        return find(mesh->getPath());
    }

    poseidon::Mesh* MeshGpuCache::find(std::string key)
    {
        auto it = m_cache.find(key);
        if (it != m_cache.end() && it->second) {
            return it->second.get();
        }
        return nullptr;
    }
}