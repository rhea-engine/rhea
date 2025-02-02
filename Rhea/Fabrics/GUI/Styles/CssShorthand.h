#ifndef RHEA_GUI_STYLE_SHORTHAND_H
#define RHEA_GUI_STYLE_SHORTHAND_H

#include "Property.h"

#include <functional>
#include <vector>
#include <Components/SimpleParser/Variant.h>

namespace Rhea
{
	struct CssShorthand : public CssProperty
	{
		// Call the base class constructor.
		CssShorthand(StylePropertyID id, char* name, bool inherited, bool shorthand)
			: CssProperty(id, name, inherited, shorthand) {}

		virtual bool Parse(std::vector<SimpleParser::Variant>& value, std::function<void(StylePropertyID, SimpleParser::Variant)> callback) const { return true; }
	};

	struct CssPadding : public CssShorthand
	{
		CssPadding(StylePropertyID id, char* name, bool inherited, bool shorthand)
			: CssShorthand(id, name, inherited, shorthand) {}

		bool Parse(std::vector<SimpleParser::Variant>& value, std::function<void(StylePropertyID, SimpleParser::Variant)> callback) const override;
	};
}

#endif // !RHEA_GUI_STYLE_SHORTHAND_H
