#ifndef RHEA_GUI_CSSPROPERTY_H
#define RHEA_GUI_CSSPROPERTY_H

#include <Components/SimpleParser/Variant.h>

namespace Rhea
{
	enum class CssValueId
	{
		kNumeric,
		kString,
		kKeyword,
		kURI,
		kIdentifier,
		kInherit
	};

	struct CssValue
	{
		CssValueId id;
		SimpleParser::Variant value;
	};
}

#endif // !RHEA_GUI_CSSPROPERTY_H
