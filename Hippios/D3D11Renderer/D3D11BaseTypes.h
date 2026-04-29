/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HIPPIOS_D3D11_BASETYPES_H
#define HIPPIOS_D3D11_BASETYPES_H

#include <Hippios/BaseTypes.h>
#include <unordered_map>
#include <d3d11.h>

namespace hippios
{
	struct D3D11GPUHandle : public GPUHandle
	{
		size_t handle;
	};

	extern std::unordered_map<ComparisonFunction, D3D11_COMPARISON_FUNC> _comparison_functions;
}

#endif // !HIPPIOS_D3D11_BASETYPES_H
