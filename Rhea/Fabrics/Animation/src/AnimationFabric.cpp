#include <Rhea/Application.h>

#include <Rhea/Animation/AnimationFabric.h>
#include <Rhea/Animation/Animator.h>
#include <Rhea/Animation/Animation.h>

namespace rhea
{
	void AnimationFabric::Register(Application* app)
	{
		simpleparser::Type::registerType<Animator>("//meworla/animator");
		simpleparser::Type::registerType<Animation>("//meworla/animation");
		simpleparser::Type::registerType<Skeleton>("//meworla/skeleton");
	}
}