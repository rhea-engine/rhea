#include <RheaAudio/AudioSystemFabric.h>
#include <RheaAudio/MusicResource.h>
#include <RheaAudio/SoundResource.h>
#include <RheaAudio/AudioService.h>

#include <Rhea/Application.h>

namespace rhea::audio
{
	void AudioSystemFabric::Register(rhea::Application* app)
	{
		MusicResource::_register_MusicResource();
		SoundResource::_register_SoundResource();

		// register audio service
		auto* audio_service = simpleparser::Type::create<AudioService>("AudioService");
		audio_service->setup(app);
		app->registerService("audio", audio_service);
	}
}