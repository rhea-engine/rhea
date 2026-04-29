/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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
        using Builder = std::function<bool(poseidon::Core*, rhea::Mesh* , poseidon::Mesh*)>;

        poseidon::Mesh* getOrCreate(poseidon::Core* core, rhea::Mesh* meshUnit, bool dynamic_usage, const Builder& builder);
        poseidon::Mesh* find(rhea::Mesh* mesh);
        poseidon::Mesh* find(std::string key);

    private:
        std::unordered_map<std::string, std::unique_ptr<poseidon::Mesh>> m_cache;
    };
}

#endif // !RHEACLIENT_GRAPHICS_MESHGPUCACHE_H
