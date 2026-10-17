/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/ResourceStream.h>
#include <Psio/Fs/Fs.h>
#include <Psio/Net/IOBuffer.h>
#include <Rhea/Gltf/GltfScene.h>

#include <Rhea/Gltf/GltfLoader.h>
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINIGLTF_NO_FS
#define TINYGLTF_IMPLEMENTATION
#include <stb_image.h>
#include <nlohmann/json.hpp>
#include <tiny_gltf_loader/tiny_gltf.h>
#include <Psio/Log.h>
#include <Rhea/Resources/ImageResource.h>

namespace rhea
{	
	bool GltfImageLoader(tinygltf::Image* image,
		int image_idx,
		std::string* err,
		std::string* warn,
		int req_width,
		int req_height,
		const unsigned char* bytes,
		int size,
		void* user_data) {
		
		GltfScene* scene = (GltfScene*)user_data;

		std::string name = image->name;
		if (name == "" && image->uri != "")
			name = image->uri;
		if (name == "")
			name = "image" + std::to_string(image_idx);

		bool success = true;
		ImageResource* img_res = new ImageResource(name, "//meworla/image");
		img_res->setup(scene->getApplication());
		char* dest = (char*)malloc(size);
		if (!dest) {
			success = false;
		}
		else {
			memcpy(dest, bytes, size);
			img_res->setRaw(size, dest);
		}

		if (!success) {
			delete img_res;
			if (err) {
				*err = "Failed to load image.";
			}
			return false;
		}
		// add to scene
		scene->addChild(img_res);
		// store index
		scene->setImage(image_idx, img_res);

		return true;
	}

	void GltfLoader::load_impl(std::string name, Application* app, orion::ResourceRequest* req, ResourceContext& context, Resource::LoadedHandler handler, bool is_binary)
	{
		if (req) {

			orion::ResourceStream* rs = req->getStream();
			size_t length = rs->getLength();
			if (!length)
				length = 64 * 1024; // default buffer size
			psio::IOBuffer* buf = new psio::IOBuffer(length);
			rs->SetMode(psio::Fs::READ | psio::Fs::BINARY);
			rs->onEnd([name, app, buf, handler, is_binary](size_t bytes_read) {
								
				GltfScene* scene = new GltfScene(name, "//meworla/gltf");
				scene->setup(app, {}, {});

				tinygltf::Model model;
				tinygltf::TinyGLTF loader;

				std::string err;
				std::string warn;

				// image loader
				loader.SetImageLoader(GltfImageLoader, (void*)scene);

				bool ret = false;
				if (is_binary)
					ret = loader.LoadBinaryFromMemory(&model, &err, &warn, (unsigned char*)buf->reader(), (unsigned int)buf->Remaining(), "");
				else
					ret = loader.LoadASCIIFromString(&model, &err, &warn, buf->reader(), (unsigned int)buf->Remaining(), "");

				if (!warn.empty())
					LOG(LOG_INFO) << warn;
				if (!err.empty())
					LOG(LOG_ERROR) << err;
				if (!ret) {
					handler(ReqError::DATA_INVALID, nullptr);
					delete buf;
					delete scene;
					return;
				}

				scene->SetModel(model);
				delete buf;

				handler(ReqError::OK, scene);
				});
			rs->onError([buf, handler](int err) {
				delete buf;
				handler(ReqError::DATA_INVALID, nullptr);
				});
			rs->ReadAsync(buf);
		}
		else {
			handler(ReqError::RESOURCE_NOT_FOUND, nullptr);
		}
	}
}
