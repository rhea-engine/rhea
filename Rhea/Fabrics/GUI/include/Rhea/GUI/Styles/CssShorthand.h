/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_STYLE_SHORTHAND_H
#define RHEA_GUI_STYLE_SHORTHAND_H

#include "Property.h"

#include <functional>
#include <vector>
#include <SimpleParser/Variant.h>
#include "CssValue.h"

namespace rhea
{
	struct CssShorthand : public CssProperty
	{
		// Call the base class constructor.
		CssShorthand(StylePropertyId id, char* name, bool inherited, bool shorthand)
			: CssProperty(id, name, inherited, shorthand) {}

		virtual bool Parse(std::vector<CssValue>& value, std::function<void(StylePropertyId, std::vector<CssValue>&)> callback) const { return true; }
	};

	struct CssPadding : public CssShorthand
	{
		CssPadding(StylePropertyId id, char* name, bool inherited, bool shorthand)
			: CssShorthand(id, name, inherited, shorthand) {}

		bool Parse(std::vector<CssValue>& value, std::function<void(StylePropertyId, std::vector<CssValue>&)> callback) const override;
	};
}

#endif // !RHEA_GUI_STYLE_SHORTHAND_H
