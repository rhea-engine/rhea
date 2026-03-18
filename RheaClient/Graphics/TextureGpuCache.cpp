#include <Rhea/Graphics/TextureGpuCache.h>
#include <Psio/Log.h>

namespace Rhea
{
    GraphicsTexture* TextureGpuCache::getOrCreate(Poseidon::Core* core, Rhea::ImageResource* img)
    {
        if (!core || !img) return nullptr;

        auto key = img->getPathName();
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            // Return the pointer to the cache entry
            return it->second.get();
        }

        auto elem = std::make_unique<GraphicsTexture>(core, img->getTextureType(), img->getNumImages());       


        if (img->getNumImages() == 1) {
            
            char* buf = nullptr;
            size_t buflen = 0;

            Resource* res = doCast<Resource>(img->getImage(0));
            if (!res) {
                LOG(LOG_ERROR) << "TextureGpuCache::getOrCreate: image[0] is not a resource";
                return nullptr;
            }
            if (res->getRaw(&buf, &buflen)) {
                if (!buf || !elem->createTexture(0, (unsigned char*)buf, buflen)) {
                    LOG(LOG_ERROR) << "TextureGpuCache::getOrCreate: texture for " << img->getPath() << " could not be created";                    
                    return nullptr;
                }
                // store the width / height
                img->width = elem->get()->getWidth();
                img->height = elem->get()->getHeight();
            }
            else {
                LOG(LOG_ERROR) << "TextureGpuCache::getOrCreate: getRaw failed: " << img->getPath();
                return nullptr;
            }
        }
        else {
            for (int index = 0; index < img->getNumImages(); index++) {
                ImageResource* child_res = doCast<ImageResource>(img->getImage(index));
                if (!child_res) {
                    LOG(LOG_ERROR) << "TextureGpuCache::getOrCreate: image[" << index << "] is not an ImageResource " << img->getPath();
                    return nullptr;
                }
                GraphicsTexture* childTex = getOrCreate(core, child_res);
                if (!childTex) {
                    LOG(LOG_ERROR) << "TextureGpuCache::getOrCreate: child texture build failed for index " << index << " of " << img->getPath();
                    return nullptr;
                }

                if (!elem->createTexture(index, childTex)) {
                    LOG(LOG_ERROR) << "TextureGpuCache::getOrCreate: could not add child texture index " << index << " for " << img->getPath();                    
                    return nullptr;
                }
            }
        }

        auto* ret = elem.get();
        m_cache.emplace(key, std::move(elem));
        return ret;
    }

    void TextureGpuCache::invalidate(Rhea::ImageResource* img)
    {
    }
}