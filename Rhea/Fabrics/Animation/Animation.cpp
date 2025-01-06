#include "Animation.h"
#include <Math/Interpolation.h>
#include <Psio/Log.h>

namespace Rhea
{
	void Animation::UpdateSkeleton(Skeleton* skeleton, float currentTime)
	{
#ifdef DEBUG_ANIMATION
		LOG(LOG_DEBUG) << "current time: " << currentTime;
#endif
		for (const auto& channel : channels) {
			if (channel.targetNode >= skeleton->getBones().size())
				continue;
			Bone& bone = skeleton->getBones()[channel.targetNode];

			// Find surrounding timestamps for translation, rotation, and scale
			Math::Vector3 interpolatedTranslation = InterpolateTranslation(channel, currentTime);
			Math::Quaternion interpolatedRotation = InterpolateRotation(channel, currentTime);
			Math::Vector3 interpolatedScale = InterpolateScale(channel, currentTime);

			// Compose the final local transform
			bone.local_transform = Math::Matrix4::compose(interpolatedTranslation, interpolatedRotation, interpolatedScale);

#ifdef DEBUG_ANIMATION
			LOG(LOG_DEBUG) << "Bone: " << bone.name;

			LOG(LOG_DEBUG) << "Translation: (" << interpolatedTranslation.x << ", " << interpolatedTranslation.y << ", " << interpolatedTranslation.z << ")";
			LOG(LOG_DEBUG) << "Rotation: (" << interpolatedRotation.w << ", " << interpolatedRotation.x << ", " << interpolatedRotation.y << ", " << interpolatedRotation.z << ")";
			LOG(LOG_DEBUG) << "Translation: (" << interpolatedScale.x << ", " << interpolatedScale.y << ", " << interpolatedScale.z << ")";
			LOG(LOG_DEBUG) << "Global Transform";
			LOG(LOG_DEBUG) << "[ " << bone.global_transform[0][0] << ", " << bone.global_transform[0][1] << ", " << bone.global_transform[0][2] << ", " << bone.global_transform[0][3] << "]";
			LOG(LOG_DEBUG) << "[ " << bone.global_transform[1][0] << ", " << bone.global_transform[1][1] << ", " << bone.global_transform[1][2] << ", " << bone.global_transform[1][3] << "]";
			LOG(LOG_DEBUG) << "[ " << bone.global_transform[2][0] << ", " << bone.global_transform[2][1] << ", " << bone.global_transform[2][2] << ", " << bone.global_transform[2][3] << "]";
			LOG(LOG_DEBUG) << "[ " << bone.global_transform[3][0] << ", " << bone.global_transform[3][1] << ", " << bone.global_transform[3][2] << ", " << bone.global_transform[3][3] << "]";
			LOG(LOG_DEBUG) << "Local Transform";
			LOG(LOG_DEBUG) << "[ " << bone.local_transform[0][0] << ", " << bone.local_transform[0][1] << ", " << bone.local_transform[0][2] << ", " << bone.local_transform[0][3] << "]";
			LOG(LOG_DEBUG) << "[ " << bone.local_transform[1][0] << ", " << bone.local_transform[1][1] << ", " << bone.local_transform[1][2] << ", " << bone.local_transform[1][3] << "]";
			LOG(LOG_DEBUG) << "[ " << bone.local_transform[2][0] << ", " << bone.local_transform[2][1] << ", " << bone.local_transform[2][2] << ", " << bone.local_transform[2][3] << "]";
			LOG(LOG_DEBUG) << "[ " << bone.local_transform[3][0] << ", " << bone.local_transform[3][1] << ", " << bone.local_transform[3][2] << ", " << bone.local_transform[3][3] << "]";
#endif			
		}	
		skeleton->UpdateBoneTransforms();  // Recompute global and final transforms for the skeleton
	}
	Math::Vector3 Animation::InterpolateTranslation(const AnimationChannel& channel, float currentTime) 
	{
		// check if only one position
		if (channel.keyframe_positions.size() == 1)
			return channel.keyframe_positions[0].translation;

		// find the surrounding keyframes
		int index = -1;
		if (channel.keyframe_positions[0].time < currentTime)
			for (index = 0; index < (int)channel.keyframe_positions.size() - 1; ++index)
			{
				if (currentTime >= channel.keyframe_positions[index].time && currentTime < channel.keyframe_positions[index + 1].time)
					break;
			}	
		
		// Handle edge cases (e.g., first or last keyframe)
		if (index < 0 || index >= channel.keyframe_positions.size() - 1) {
			return (index < 0) ? channel.keyframe_positions.front().translation : channel.keyframe_positions.back().translation;
		}

		// Interpolate between the two keyframes
		float t = NormalizeTime(channel.keyframe_positions[index].time, channel.keyframe_positions[index + 1].time, currentTime);		
		return Math::lerp(channel.keyframe_positions[index].translation, channel.keyframe_positions[index + 1].translation, t);
	}

	Math::Quaternion Animation::InterpolateRotation(const AnimationChannel& channel, float currentTime)
	{
		// check if only one position
		if (channel.keyframe_rotations.size() == 1)
			return channel.keyframe_rotations[0].rotation;

		// find the surrounding keyframes
		int index = -1;
		if (channel.keyframe_rotations[0].time < currentTime)
			for (index = 0; index < (int)channel.keyframe_rotations.size() - 1; ++index)
			{
				if (currentTime >= channel.keyframe_rotations[index].time && currentTime < channel.keyframe_rotations[index + 1].time)
					break;
			}		

		// Handle edge cases (e.g., first or last keyframe)
		if (index < 0 || index >= channel.keyframe_rotations.size() - 1) {
			return (index < 0) ? channel.keyframe_rotations.front().rotation : channel.keyframe_rotations.back().rotation;
		}

		// Interpolate between the two keyframes
		float t = NormalizeTime(channel.keyframe_rotations[index].time, channel.keyframe_rotations[index + 1].time, currentTime);		
		return Math::slerp(channel.keyframe_rotations[index].rotation, channel.keyframe_rotations[index + 1].rotation, t);
	}

	Math::Vector3 Animation::InterpolateScale(const AnimationChannel& channel, float currentTime) 
	{
		// check if only one position
		if (channel.keyframe_scales.size() == 1)
			return channel.keyframe_scales[0].scale;

		// find the surrounding keyframes
		int index = -1;
		if (channel.keyframe_scales[0].time < currentTime)
			for (index = 0; index < (int)channel.keyframe_scales.size() - 1; ++index)
			{
				if (currentTime >= channel.keyframe_scales[index].time && currentTime < channel.keyframe_scales[index + 1].time)
					break;
			}		

		// Handle edge cases (e.g., first or last keyframe)
		if (index < 0 || index >= channel.keyframe_scales.size() - 1) {
			return (index < 0) ? channel.keyframe_scales.front().scale : channel.keyframe_scales.back().scale;
		}

		// Interpolate between the two keyframes
		float t = NormalizeTime(channel.keyframe_scales[index].time, channel.keyframe_scales[index + 1].time, currentTime);		
		return Math::lerp(channel.keyframe_scales[index].scale, channel.keyframe_scales[index + 1].scale, t);
	}

	/* Gets normalized value for Lerp & Slerp
	*/
	float Animation::NormalizeTime(float startTime, float endTime, float currentTime) {
		return (currentTime - startTime) / (endTime - startTime);
	}
}