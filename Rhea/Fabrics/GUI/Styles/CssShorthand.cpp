#include "CssShorthand.h"

namespace Rhea
{
	bool CssPadding::Parse(std::vector<CssValue>& value, std::function<void(StylePropertyId, std::vector<CssValue>&)> callback) const
	{
		std::vector<CssValue> top;
		std::vector<CssValue> left;
		std::vector<CssValue> bottom;
		std::vector<CssValue> right;

		switch (value.size()) {
		case 1:
			top.push_back(value[0]);
			left.push_back(value[0]);
			bottom.push_back(value[0]);
			right.push_back(value[0]);
			break;
		case 2:
			top.push_back(value[0]);
			left.push_back(value[1]);
			bottom.push_back(value[0]);
			right.push_back(value[1]);
			break;
		case 3:
			top.push_back(value[0]);
			left.push_back(value[1]);
			bottom.push_back(value[2]);
			right.push_back(value[1]);
			break;
		case 4:
			top.push_back(value[0]);
			left.push_back(value[3]);
			bottom.push_back(value[2]);
			right.push_back(value[1]);
			break;
		default:
			top.push_back(value[0]);
			left.push_back(value[0]);
			bottom.push_back(value[0]);
			right.push_back(value[0]);
		}
		
		callback(StylePropertyId::kPaddingTop, top);
		callback(StylePropertyId::kPaddingLeft, left);
		callback(StylePropertyId::kPaddingBottom, bottom);
		callback(StylePropertyId::kPaddingRight, right);

		return true;
	}
}