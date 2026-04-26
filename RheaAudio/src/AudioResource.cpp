#include <RheaAudio/AudioResource.h>

namespace rhea::audio
{
	AudioResource::~AudioResource()
	{
	}

	void AudioResource::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Resource::setup(app, attributes, values);
	}

	void AudioResource::ready()
	{
		Resource::ready();
	}

	std::vector<Unit::Export> AudioResource::getReferences()
	{
		return {};
	}
}