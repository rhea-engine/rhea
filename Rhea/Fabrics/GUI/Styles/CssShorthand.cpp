#include "CssShorthand.h"

namespace Rhea
{
	bool CssPadding::Parse(std::vector<SimpleParser::Variant>& value, std::function<void(StylePropertyID, SimpleParser::Variant)> callback) const
	{
		SimpleParser::Variant top;
		SimpleParser::Variant left;
		SimpleParser::Variant bottom;
		SimpleParser::Variant right;

		switch (value.size()) {
		case 1:
			top = value[0];
			left = value[0];
			bottom = value[0];
			right = value[0];
			break;
		case 2:
			top = value[0];
			left = value[1];
			bottom = value[0];
			right = value[1];
			break;
		case 3:
			top = value[0];
			left = value[1];
			bottom = value[2];
			right = value[1];
			break;
		case 4:
			top = value[0];
			left = value[3];
			bottom = value[2];
			right = value[1];
			break;
		default:
			top = value[0];
			left = value[0];
			bottom = value[0];
			right = value[0];
		}
		callback(StylePropertyID::kPaddingTop, std::stof(top));
		callback(StylePropertyID::kPaddingLeft, std::stof(left));
		callback(StylePropertyID::kPaddingBottom, std::stof(bottom));
		callback(StylePropertyID::kPaddingRight, std::stof(right));

		return true;
	}
}