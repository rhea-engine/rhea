#include <Orion/ResourceStream.h>
#include <Psio/Fs/Fs.h>
#include <Psio/Net/IOBuffer.h>
#include <Rhea/Fabrics/Gltf/GltfScene.h>

#include "GltfLoader.h"
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINIGLTF_NO_FS
#define TINYGLTF_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <nlohmann/json.hpp>
#include <ThirdParty/tiny_gltf_loader/tiny_gltf.h>
#include <Psio/Log.h>
#include <Fabrics/Media/ImageResource.h>

namespace Rhea
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

	void GltfLoader::load_impl(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler, bool is_binary)
	{
		if (req) {

			Orion::ResourceStream* rs = req->getStream();
			size_t length = rs->getLength();
			if (!length)
				length = 64 * 1024; // default buffer size
			Psio::IOBuffer* buf = new Psio::IOBuffer(length);
			rs->SetMode(Psio::Fs::READ | Psio::Fs::BINARY);
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
