/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "D3D11BaseTypes.h"

namespace hippios {
	std::unordered_map<ComparisonFunction, D3D11_COMPARISON_FUNC> _comparison_functions = {
		{ ComparisonFunction::COMPARE_NEVER, D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER },
		{ ComparisonFunction::COMPARE_LESS, D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS },
		{ ComparisonFunction::COMPARE_EQUAL, D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL },
		{ ComparisonFunction::COMPARE_LESS_EQUAL, D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL },
		{ ComparisonFunction::COMPARE_GREATER, D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER },
		{ ComparisonFunction::COMPARE_NOT_EQUAL, D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL },
		{ ComparisonFunction::COMPARE_GREATER_EQUAL, D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL },
		{ ComparisonFunction::COMPARE_ALWAYS, D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS }
	};
}
