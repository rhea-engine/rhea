/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_CHARACTERFABRIC_H
#define RHEA_CHARACTERFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea
{
	/** Fabrics for characters
	*/
	class CharacterFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Character"; }
	};
}

#endif // !RHEA_CHARACTERFABRICS_H
