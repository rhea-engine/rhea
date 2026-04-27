#include <RheaAudio/MusicResource.h>

namespace rhea::audio
{
	std::vector<Unit::Export> MusicResource::getReferences()
	{
		std::vector<Export> refs;
		if (m_values.find("path") != m_values.end()) {
			refs.push_back({ "audio", "//meworla/audio", m_values["path"] });
		}
		return refs;
	}
}