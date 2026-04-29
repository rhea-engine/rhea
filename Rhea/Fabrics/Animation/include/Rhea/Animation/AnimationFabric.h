/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_ANIMATIONFABRIC_H
#define RHEA_ANIMATIONFABRIC_H

namespace rhea
{
	class Application;

	/** Fabrics for animations
	*/
	class AnimationFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Animation"; }
	};
}

#endif // !RHEA_ANIMATIONFABRIC_H
