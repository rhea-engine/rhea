#ifndef RHEA_ANIMATION_H
#define RHEA_ANIMATION_H

#include <Rhea/Unit.h>
#include <Rhea/Fabrics/3D/Skeleton.h>

namespace Rhea
{
	class Animation : public Unit
	{
		MECLASS(Animation, Unit)
	public:
		
		Animation(std::string name, std::string typename) :
			duration(0.0f),
			Unit(name, "//meworla/animation")
		{}

		struct Keyframe {
			float time;
			Math::Vector3 translation;
			Math::Quaternion rotation;
			Math::Vector3 scale;
		};

		struct AnimationChannel {
			int targetNode;  // Index of the target node (e.g., bone)
			std::vector<Keyframe> keyframes;  // Keyframes for this channel
		};

		float duration;

		std::vector<AnimationChannel> channels;  // Channels controlling different nodes

		void UpdateSkeleton(Skeleton* skeleton, float currentTime);

		void FindSurroundingKeyframes(const AnimationChannel& channel, float time, Keyframe& prev, Keyframe& next);
	};
}
#endif // !RHEA_ANIMATION_H
