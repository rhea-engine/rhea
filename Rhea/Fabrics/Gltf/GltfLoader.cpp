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

namespace Rhea
{	
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
				// make a rhea mesh from the model?
				Rhea::Mesh* mesh = new Rhea::Mesh("mesh");
				scene->load(mesh);
				scene->addChild(mesh);

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
