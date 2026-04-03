#ifndef RHEA_ANIMATION_H
#define RHEA_ANIMATION_H

#include <Rhea/Unit.h>
#include "Skeleton.h"

namespace Rhea
{
	class Animation : public Unit
	{
		MECLASS(Animation, Unit)
	public:
		
		Animation(std::string name, std::string type_name) :
			duration(0.0f),
			Unit(name, "//meworla/animation")
		{}

		struct KeyframePosition {
			float time = 0.0f;
			Math::Vector3 translation;
		};
		struct KeyframeRotation {
			float time = 0.0f;
			Math::Quaternion rotation;
		};
		struct KeyframeScale {
			float time = 0.0f;
			Math::Vector3 scale = { 1.0f, 1.0f, 1.0f };
		};

		struct AnimationChannel {
			int targetNode;  // Index of the target node (e.g., bone)
			std::vector<KeyframePosition> keyframe_positions;  // Keyframes for this channel
			std::vector<KeyframeRotation> keyframe_rotations;
			std::vector<KeyframeScale> keyframe_scales;
		};

		float duration;

		std::vector<AnimationChannel> channels;  // Channels controlling different nodes

		void UpdateSkeleton(Skeleton* skeleton, float currentTime);

	protected:

		Math::Vector3 InterpolateTranslation(const AnimationChannel& channel, float currentTime);

		Math::Quaternion InterpolateRotation(const AnimationChannel& channel, float currentTime);

		Math::Vector3 InterpolateScale(const AnimationChannel& channel, float currentTime);

		float NormalizeTime(float startTime, float endTime, float currentTime);		
	};
}
#endif // !RHEA_ANIMATION_H
