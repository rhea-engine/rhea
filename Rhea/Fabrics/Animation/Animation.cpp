#include "Animation.h"
#include <Math/Interpolation.h>
#include <Psio/Log.h>

namespace Rhea
{
	void Animation::UpdateSkeleton(Skeleton* skeleton, float currentTime)
	{
		LOG(LOG_DEBUG) << "current time: " << currentTime;
		for (const auto& channel : channels) {
			if (channel.targetNode >= skeleton->getBones().size())
				continue;
			Bone& bone = skeleton->getBones()[channel.targetNode];

			Keyframe prevKeyframe, nextKeyframe;
			FindSurroundingKeyframes(channel, currentTime, prevKeyframe, nextKeyframe);
			if (channel.keyframes.size()>2)
				LOG(LOG_DEBUG) << "Target(" << channel.targetNode << ") Prev Keyframe Time: " << prevKeyframe.time << ", Next Keyframe Time: " << nextKeyframe.time;

			// Normalize currentTime relative to the prevKeyframe.time and nextKeyframe.time
			float duration = nextKeyframe.time - prevKeyframe.time;
			float t = (duration > 0.0f) ? (currentTime - prevKeyframe.time) / duration : 0.0f;

			// Interpolate translation
			Math::Vector3 interpolatedTranslation = Math::lerp<Math::Vector3>(prevKeyframe.translation, nextKeyframe.translation, t);

			// Interpolate rotation
			Math::Quaternion interpolatedRotation = Math::slerp(prevKeyframe.rotation, nextKeyframe.rotation, t);

			// Interpolate scale
			Math::Vector3 interpolatedScale = Math::lerp<Math::Vector3>(prevKeyframe.scale, nextKeyframe.scale, t);

			// Build local transform
			bone.local_transform = Math::Matrix4::compose(interpolatedTranslation, interpolatedRotation, interpolatedScale);
			if (channel.keyframes.size() > 2)
			{
				LOG(LOG_DEBUG) << "m[0][0]=" << bone.local_transform[0][0] << ",m[0][1]=" << bone.local_transform[0][1] << ",m[0][2]=" << bone.local_transform[0][2] << ",m[0][3]=" << bone.local_transform[0][3];
				LOG(LOG_DEBUG) << "m[1][0]=" << bone.local_transform[1][0] << ",m[1][1]=" << bone.local_transform[1][1] << ",m[1][2]=" << bone.local_transform[1][2] << ",m[1][3]=" << bone.local_transform[1][3];
				LOG(LOG_DEBUG) << "m[2][0]=" << bone.local_transform[2][0] << ",m[2][1]=" << bone.local_transform[2][1] << ",m[2][2]=" << bone.local_transform[2][2] << ",m[2][3]=" << bone.local_transform[2][3];
				LOG(LOG_DEBUG) << "m[3][0]=" << bone.local_transform[3][0] << ",m[3][1]=" << bone.local_transform[3][1] << ",m[3][2]=" << bone.local_transform[3][2] << ",m[3][3]=" << bone.local_transform[3][3];
			}
		}

		skeleton->UpdateBoneTransforms();  // Recompute global and final transforms for the skeleton
	}

	void Animation::FindSurroundingKeyframes(const AnimationChannel& channel, float time, Keyframe& prev, Keyframe& next)
	{
		// Handle time before the first keyframe
		if (time <= channel.keyframes.front().time) {
			prev = next = channel.keyframes.front();
			return;
		}

		// Handle time after the last keyframe
		if (time >= channel.keyframes.back().time) {
			prev = next = channel.keyframes.back();
			return;
		}

		for (size_t i = 0; i < channel.keyframes.size() - 1; ++i) {
			if (time >= channel.keyframes[i].time && time < channel.keyframes[i + 1].time) {
				prev = channel.keyframes[i];
				next = channel.keyframes[i + 1];
				return;
			}
		}
	}
}