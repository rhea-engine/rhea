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

	void AudioResource::load(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		if (req) {
			std::string mime = req->getMimeType();

			psio::Stream* rs = req->getStream();
			psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
			rs->SetMode(psio::Fs::READ | psio::Fs::BINARY);
			rs->onEnd([buf, app, name, mime,handler](size_t buflen) {

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
					delete buf;
					handler(ReqError::OK, res);
				}
				else {
					delete buf;
					handler(ReqError::DATA_INVALID, nullptr);
				}
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

	std::vector<Unit::Export> AudioResource::getReferences()
	{
		std::vector<Export> refs;
		if (m_values.find("path") != m_values.end()) {
			refs.push_back({ "audio", "//meworla/audio", m_values["path"] });
		}
		return refs;
	}
}