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
