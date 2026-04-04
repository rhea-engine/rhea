#ifndef RHEA_TEXTUREGPUCACHE_H
#define RHEA_TEXTUREGPUCACHE_H

#include <Poseidon/Texture.h>
#include <Poseidon/Core.h>
#include <Rhea/Resources/ImageResource.h>
#include <Rhea/Graphics/Texture.h>

namespace rhea::client {
    class TextureGpuCache {
    public:
        GraphicsTexture* getOrCreate(Poseidon::Core* core, rhea::ImageResource* img);

        Poseidon::Texture* getPoseidonTexture(Poseidon::Core* core, rhea::ImageResource* img) {
            auto* gt = getOrCreate(core, img);
            return gt ? (Poseidon::Texture*)gt->get() : nullptr;
        }

        void invalidate(rhea::ImageResource* img);
        void clear();

    private:
        std::unordered_map<std::string, std::unique_ptr<GraphicsTexture>> m_cache;
    };
}
#endif // !RHEA_TEXTUREGPUCACHE_H
