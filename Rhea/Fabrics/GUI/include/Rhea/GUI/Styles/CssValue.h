/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_CSSPROPERTY_H
#define RHEA_GUI_CSSPROPERTY_H

#include <SimpleParser/Variant.h>

namespace rhea
{
	enum class CssValueId
	{
		kNumeric,
		kPercentage,
		kDimensionEm,
		kString,
		kKeyword,
		kURI,
		kIdentifier,
		kInherit
	};

	struct CssValue
	{
		CssValueId id;
		simpleparser::Variant value;
	};
}

#endif // !RHEA_GUI_CSSPROPERTY_H
