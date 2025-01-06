#include <Rhea/Application.h>

#include "AnimationFabrics.h"
#include "Animator.h"
#include "Animation.h"

namespace Rhea
{
	void AnimationFabrics::Register(Application* app)
	{
		SimpleParser::Type::registerType<Animator>("//meworla/animator");
		SimpleParser::Type::registerType<Animation>("//meworla/animation");
		SimpleParser::Type::registerType<Skeleton>("//meworla/skeleton");
	}
}