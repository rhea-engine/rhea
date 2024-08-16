#include <Rhea/Application.h>

#include "AnimationFabrics.h"
#include "Animation.h"

namespace Rhea
{
	void AnimationFabrics::Register(Application* app)
	{
		SimpleParser::Type::registerType<Animation>("//meworla/animation");
	}
}