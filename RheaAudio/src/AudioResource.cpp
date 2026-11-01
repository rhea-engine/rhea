/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <RheaAudio/AudioResource.h>
#include <Orion/ResourceStream.h>
#include <Psio/Fs/Fs.h>
#include <RheaAudio/MusicResource.h>
#include <RheaAudio/SoundResource.h>
#include <RheaAudio/AudioService.h>
#include <Rhea/Application.h>

#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <cstring>

namespace rhea::audio
{
	AudioResource::~AudioResource()
	{
	}

	void AudioResource::onDestroy()
	{
		// get audio service reference and tell it to unload this resource from the backend
		AudioService* audio_service = doCast<AudioService>(m_app->getService("audio"));
		if (audio_service)
		{
			audio_service->unload(this);
		}
	}

	void AudioResource::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Resource::setup(app, attributes, values);
	}

	void AudioResource::ready()
	{
		Resource::ready();
	}

	void AudioResource::load(const std::string& name, const std::string& res_id, Application* app, orion::ResourceRequest* req, ResourceContext context, Resource::LoadedHandler handler)
	{
		if (req) {
			std::string mime = req->getMimeType();

			psio::Stream* rs = req->getStream();
			psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
			rs->SetMode(psio::Fs::READ | psio::Fs::BINARY);
			rs->onEnd([buf, app, name, mime, res_id, handler](size_t buflen) {

				if (buflen > 0) {
					Resource* res = nullptr;
					std::string subtype = mime.substr(mime.find_last_of("/") + 1);
					if (subtype == "mpeg" || subtype == "mp3") {
						res = new MusicResource(name, "//meworla/music");
					}
					else if (subtype == "wav" || subtype == "ogg") {
						res = new SoundResource(name, "//meworla/sound");
					}
					else {
						// Fallback to generic AudioResource
						res = new AudioResource(name, "//meworla/audio");
					}
					char* dest = (char*)malloc(buf->Remaining());
					if (dest) {
						memcpy(dest, buf->reader(), buf->Remaining());
						res->setRaw(buf->Remaining(), dest);
						res->setup(app);
					}
					else {
						delete buf;
						delete res;
						if (handler)
							handler(Error(Error::Code::UNAVAILABLE, "Could not allocate memory for audio resource", res_id), nullptr);
						return;
					}
					delete buf;
					if (handler)
						handler(Error(), res);
				}
				else {
					delete buf;
					if (handler)
						handler(Error(Error::Code::RESOURCE_ERROR, "Could not read audio resource", res_id), nullptr);
				}
				});
			rs->onError([buf, res_id, handler](int err) {
				delete buf;
				Error error(Error::Code::RESOURCE_ERROR, "Could not read audio resource", res_id);
				error.addContext("stream_error", std::to_string(err));
				if (handler)
					handler(error, nullptr);
				});
			rs->ReadAsync(buf);
		}
		else {
			handler(Error(Error::Code::NOT_FOUND, "Resource not found"), nullptr);
		}
	}

	std::vector<Unit::ResourceReference> AudioResource::getReferences()
	{
		std::vector<ResourceReference> refs;
		if (m_values.find("path") != m_values.end()) {
			refs.push_back({ "audio", "//meworla/audio", m_values["path"] });
		}
		return refs;
	}
}