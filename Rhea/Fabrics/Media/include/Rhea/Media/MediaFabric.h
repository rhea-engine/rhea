/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_MEDIA_FABRIC_H
#define RHEA_MEDIA_FABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea
{
	class Application;

	/** Fabrics for basic image loading
	*/
	class MediaFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;
		const char* getName() const override { return "Media"; }
	};
}

#endif // !RHEA_MEDIA_FABRIC_H
