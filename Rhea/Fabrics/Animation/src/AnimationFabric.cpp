#include <Rhea/Application.h>

#include <Rhea/Animation/AnimationFabric.h>
#include <Rhea/Animation/Animator.h>
#include <Rhea/Animation/Animation.h>

namespace Rhea
{
	void AnimationFabric::Register(Application* app)
	{
		SimpleParser::Type::registerType<Animator>("//meworla/animator");
		SimpleParser::Type::registerType<Animation>("//meworla/animation");
		SimpleParser::Type::registerType<Skeleton>("//meworla/skeleton");
	}
}