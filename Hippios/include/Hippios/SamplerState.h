/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HIPPIOS_SAMPLER_H
#define HIPPIOS_SAMPLER_H

#include "BaseTypes.h"
#include <cfloat>

namespace hippios
{	
	struct SamplerDesc
	{
		enum class Filter {
			LINEAR,
			MIP_POINT, // Nearest-neighbor filtering
		};

		enum class AddressMode {
			WRAP,
			CLAMP
		};

		Filter filter = Filter::LINEAR;
		AddressMode address_u = AddressMode::WRAP;
		AddressMode address_v = AddressMode::WRAP;
		AddressMode address_w = AddressMode::WRAP;
		ComparisonFunction comparison_func = ComparisonFunction::COMPARE_NEVER;
		float min_lod = 0;
		float max_lod = FLT_MAX;
	};

	class SamplerState
	{
	public:
		virtual ~SamplerState() = default;
	};
}

#endif // !HIPPIOS_SAMPLER_H
