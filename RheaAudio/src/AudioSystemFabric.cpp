#include <RheaAudio/AudioSystemFabric.h>
#include <RheaAudio/MusicResource.h>
#include <RheaAudio/SoundResource.h>
#include <RheaAudio/AudioService.h>

#include <Rhea/Application.h>
#include <Orion/Mime.h>
#include <Psio/Log.h>

namespace rhea::audio
{
	void AudioSystemFabric::Register(rhea::Application* app)
	{
		AudioResource::_register_AudioResource();
		MusicResource::_register_MusicResource();
		SoundResource::_register_SoundResource();

		// register mime type for shader
		orion::MIME::RegisterExtenstion("audio-x/wav", "wav");
		orion::MIME::RegisterExtenstion("audio-x/ogg", "ogg");
		orion::MIME::RegisterExtenstion("audio/mpeg", "mp3");

		auto* resource_loader = app->getResourceLoader();
		resource_loader->registerLoader<AudioResource>("audio-x/wav");

		// register audio service
		auto* audio_service = simpleparser::Type::create<AudioService>("AudioService");
		audio_service->setup(app);
		if (!audio_service->initialize()) {
			LOG(LOG_ERROR) << "Failed to initialize audio service";
		}
		else
			app->registerService("audio", audio_service);
	}
}