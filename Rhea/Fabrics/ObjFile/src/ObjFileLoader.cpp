#include <Rhea/ObjFile/ObjFileLoader.h>
#include <Rhea/ObjFile/ObjDocument.h>
#include <Orion/ResourceStream.h>
#include <Psio/Fs/Fs.h>
#include <Psio/Net/IOBuffer.h>

namespace rhea
{
	void ObjFileLoader::load(const std::string& name, const std::string& res_id, Application* app, orion::ResourceRequest* req, ResourceContext context, Resource::LoadedHandler handler)
	{
		if (!req || !req->getStream()) {
			if (handler)
				handler(Error(Error::Code::NOT_FOUND, "Resource stream not found", res_id), nullptr);
			return;
		}

		auto* stream = req->getStream();
		auto buffer = std::make_shared<psio::IOBuffer>(stream->getLength() ? stream->getLength() : 64 * 1024);
		stream->SetMode(psio::Fs::READ | psio::Fs::BINARY);
		stream->onEnd([handler, app, name, context, buffer](size_t) {

			std::string obj_text(reinterpret_cast<const char*>(buffer->reader()), buffer->Remaining());

			auto* document = new ObjDocument(name);
			document->setup(app, {}, {});
			document->setSourceText(obj_text);
			document->setResourceContext(context);

			if (handler)
				handler(Error(), document);
			});
		stream->onError([res_id, handler, buffer](int stream_error) {
			Error error = Error(Error::Code::RESOURCE_ERROR, "Could not read OBJ resource", res_id);
			error.addContext("stream_error", std::to_string(stream_error));
			if (handler)
				handler(error, nullptr);
			});
		stream->ReadAsync(buffer.get());

	}
}