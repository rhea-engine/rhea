#ifndef RHEA_MATERIALGPUCACHE_H
#define RHEA_MATERIALGPUCACHE_H

#include <Poseidon/Core.h>
#include <Poseidon/Material.h>
#include <Rhea/Core/Material.h>
#include <functional>

namespace rhea::client {
    class MaterialGpuCache {
    public:
        using Builder = std::function<bool(Poseidon::Core*, rhea::Material* , Poseidon::Material*, bool)>;

        Poseidon::Material* getOrCreate(Poseidon::Core* core, rhea::Material* m, const Builder& builder);

    private:
        struct Entry {
            std::unique_ptr<Poseidon::Material> mat;
            std::string shaderName;
            std::vector<std::string> textureKeys;
        };
        std::vector<std::string> collectTexKeys(rhea::Material* material);
        std::unordered_map<std::string, Entry> m_entries; // key: material pathName
    };
}
#endif // !RHEA_MATERIALGPUCACHE_H
