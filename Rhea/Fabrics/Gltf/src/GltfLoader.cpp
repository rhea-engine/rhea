/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <fstream>
#include <Rhea/Gltf/GltfLoader.h>
#include <Rhea/Gltf/GltfScene.h>
#include <Rhea/Application.h>
#include <Orion/ResourceStream.h>
#include <Psio/Fs/Fs.h>
#include <Psio/Net/IOBuffer.h>
#include <Rhea/Resources/ImageResource.h>
#include <Psio/Log.h>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf_loader/tiny_gltf.h>

namespace rhea
{
    bool GltfImageLoader(tinygltf::Image* image, int image_idx, std::string* error,
        std::string*, int, int, const unsigned char* bytes, int size, void* user_data)
    {
        auto* scene = static_cast<GltfScene*>(user_data);
        if (!bytes || size <= 0) {
            if (error)
                *error = "Empty glTF image data";
            return false;
        }
        auto resource = std::make_unique<ImageResource>(
            "image_" + std::to_string(image_idx), "//meworla/image");
        resource->setup(scene->getApplication());
        char* destination = static_cast<char*>(malloc(size));
        if (!destination) {
            if (error)
                *error = "Could not allocate glTF image data";
            return false;
        }
        memcpy(destination, bytes, size);
        resource->setRaw(size, destination);
        if (!scene->addChild(resource.get())) {
            if (error)
                *error = "Could not attach glTF image resource";
            return false;
        }
        scene->setImage(image_idx, resource.release());
        return true;
    }
    namespace
    {
        using Bytes = std::vector<unsigned char>;
        using AssetMap = std::unordered_map<std::string, Bytes>;

        std::string assetKey(std::string path)
        {
            while (path.compare(0, 2, "./") == 0)
                path.erase(0, 2);
            return path;
        }

        uint32_t readUint32(const unsigned char* data)
        {
            return uint32_t(data[0]) | (uint32_t(data[1]) << 8) |
                (uint32_t(data[2]) << 16) | (uint32_t(data[3]) << 24);
        }

        nlohmann::json readDocument(const Bytes& bytes, bool binary)
        {
            if (!binary)
                return nlohmann::json::parse(bytes.begin(), bytes.end());

            if (bytes.size() < 20 || readUint32(bytes.data()) != 0x46546c67 ||
                readUint32(bytes.data() + 4) != 2 || readUint32(bytes.data() + 8) != bytes.size())
                return nlohmann::json();

            const size_t json_size = readUint32(bytes.data() + 12);
            if (readUint32(bytes.data() + 16) != 0x4e4f534a || json_size > bytes.size() - 20)
                return nlohmann::json();

            return nlohmann::json::parse(bytes.begin() + 20, bytes.begin() + 20 + json_size);
        }

        struct LoadState
        {
            Application* app = nullptr;
            ResourceContext context;
            Resource::LoadedHandler handler;
            std::unique_ptr<GltfScene> scene;
            Bytes source;
            AssetMap assets;
            size_t remaining = 0;
            bool binary = false;
            bool completed = false;
            Error error = Error();
            std::string resource_id;

            void parse()
            {
                if (completed)
                    return;
                completed = true;
                if (error.isError()) {
                    handler(error, nullptr);
                    return;
                }

                tinygltf::FsCallbacks callbacks{};
                callbacks.user_data = &assets;
                callbacks.FileExists = [](const std::string& path, void* data) {
                    const auto& files = *static_cast<AssetMap*>(data);
                    return files.find(assetKey(path)) != files.end();
                };
                callbacks.ExpandFilePath = [](const std::string& path, void*) { return assetKey(path); };
                callbacks.ReadWholeFile = [](Bytes* out, std::string* message, const std::string& path, void* data) {
                    const auto& files = *static_cast<AssetMap*>(data);
                    const auto it = files.find(assetKey(path));
                    if (it == files.end()) {
                        if (message)
                            *message = "glTF dependency was not preloaded: " + path;
                        return false;
                    }
                    *out = it->second;
                    return true;
                };
                callbacks.GetFileSizeInBytes = [](size_t* size, std::string*, const std::string& path, void* data) {
                    const auto& files = *static_cast<AssetMap*>(data);
                    const auto it = files.find(assetKey(path));
                    if (it == files.end())
                        return false;
                    *size = it->second.size();
                    return true;
                };
                callbacks.WriteWholeFile = [](std::string*, const std::string&, const Bytes&, void*) { return false; };

                tinygltf::TinyGLTF loader;
                loader.SetFsCallbacks(callbacks);
                loader.SetImageLoader(GltfImageLoader, scene.get());
                tinygltf::Model model;
                std::string message;
                std::string warning;
                const auto size = static_cast<unsigned int>(source.size());
                const bool success = binary
                    ? loader.LoadBinaryFromMemory(&model, &message, &warning, source.data(), size, "")
                    : loader.LoadASCIIFromString(&model, &message, &warning,
                        reinterpret_cast<const char*>(source.data()), size, "");
                if (!warning.empty())
                    LOG(LOG_WARNING) << warning;
                if (!message.empty())
                    LOG(LOG_ERROR) << message;
                if (!success) {
                    Error err(Error::Code::INVALID_DATA, "Failed to load glTF resource: " + message, resource_id);
                    if (handler)
                        handler(err, nullptr);
                    return;
                }
                scene->SetModel(std::move(model));
                if (handler)
                    handler(Error(), scene.release());
            }

            void finishAsset(Error result)
            {
                if (result.isError() && !error.isError())
                    error = result;
                if (--remaining == 0)
                    parse();
            }
        };
    }

    void GltfLoader::load_impl(std::string name, Application* app, orion::ResourceRequest* req,
        ResourceContext& context, Resource::LoadedHandler handler, bool is_binary)
    {
        if (!req || !req->getStream()) {
            const std::string resource_id = req ? "//" + req->getRealmName() + req->getPath() : context.normalizeResourceId(name);
            if (handler)
                handler(Error(Error::Code::NOT_FOUND, "Resource stream not found", resource_id), nullptr);
            return;
        }

        auto state = std::make_shared<LoadState>();
        state->app = app;
        state->context = context;
        state->handler = std::move(handler);
        state->binary = is_binary;
        state->scene = std::make_unique<GltfScene>(name, "//meworla/gltf");
        state->scene->setup(app, {}, {});
		state->resource_id = "//" + req->getRealmName() + req->getPath();

        auto* stream = req->getStream();
        auto buffer = std::make_shared<psio::IOBuffer>(stream->getLength() ? stream->getLength() : 64 * 1024);
        stream->SetMode(psio::Fs::READ | psio::Fs::BINARY);
        stream->onEnd([state, buffer](size_t) {
            state->source.assign(buffer->reader(), buffer->reader() + buffer->Remaining());
            if (state->source.empty() || state->source.size() > (std::numeric_limits<unsigned int>::max)()) {
                state->error = Error(Error::Code::INVALID_DATA, "glTF resource is empty or exceeds the supported size", state->resource_id);
                state->parse();
                return;
            }

            try {
                const auto document = readDocument(state->source, state->binary);
                if (!document.is_object())
                    throw std::runtime_error("Invalid glTF/GLB document");
                for (const auto* section : {"buffers", "images"}) {
                    if (!document.contains(section))
                        continue;
                    for (const auto& entry : document.at(section)) {
                        const std::string uri = entry.value("uri", std::string());
                        if (uri.empty() || uri.compare(0, 5, "data:") == 0)
                            continue;
                        std::string decoded;
                        if (!tinygltf::URIDecode(uri, &decoded, nullptr))
                            throw std::runtime_error("Invalid glTF dependency URI: " + uri);
                        state->assets.emplace(assetKey(decoded), Bytes{});
                    }
                }
            }
            catch (const std::exception& error) {
                LOG(LOG_ERROR) << "Could not inspect glTF dependencies: " << error.what();
                state->error = Error(Error::Code::INVALID_DATA, "Failed to load glTF resource: " + std::string(error.what()), state->resource_id);
                state->parse();
                return;
            }

            state->remaining = state->assets.size();
            if (!state->remaining) {
                state->parse();
                return;
            }

            // Keep dispatch independent of synchronous callbacks updating the asset contents.
            std::vector<std::string> paths;
            for (const auto& entry : state->assets)
                paths.push_back(entry.first);
            for (const auto& path : paths) {
                const std::string rid = state->context.normalizeResourceId(path);
                auto* dependency = state->app->getFS()->getResourceByRid(rid);
                dependency->resolve([state, path, rid](orion::ResourceRequest* resolved, orion::Error error) {
                    if (error != orion::Error::OK) {
                        LOG(LOG_ERROR) << "Could not load glTF dependency: " << rid;
                        Error result = translateLoadError(error, rid);
                        result.addContext("referenced_by", state->resource_id);
                        state->finishAsset(result);
                        return;
                    }
                    auto* asset_stream = resolved->getStream();
                    auto asset_buffer = std::make_shared<psio::IOBuffer>(
                        asset_stream->getLength() ? asset_stream->getLength() : 64 * 1024);
                    asset_stream->SetMode(psio::Fs::READ | psio::Fs::BINARY);
                    asset_stream->onEnd([state, path, asset_buffer](size_t) {
                        state->assets.at(path).assign(asset_buffer->reader(),
                            asset_buffer->reader() + asset_buffer->Remaining());
                        state->finishAsset(Error());

                    });
                    asset_stream->onError([state, rid, asset_buffer](int err) {
                        LOG(LOG_ERROR) << "Could not read glTF dependency: " << rid << " (stream error: " << err << ")";
                        Error result(Error::Code::RESOURCE_ERROR, "Could not read glTF dependency", rid);
						result.addContext("referenced_by", state->resource_id);
						result.addContext("stream_error", std::to_string(err));
                        state->finishAsset(result);
                    });
                    asset_stream->ReadAsync(asset_buffer.get());
                });
            }
        });
        stream->onError([state, buffer](int stream_error) {
            state->error = Error(Error::Code::RESOURCE_ERROR, "Could not read glTF resource", state->resource_id);
            state->error.addContext("stream_error", std::to_string(stream_error));
            state->parse();
        });
        stream->ReadAsync(buffer.get());
    }
}
