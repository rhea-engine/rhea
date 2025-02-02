#include "Property.h"
#include "CssShorthand.h"

namespace Rhea
{
    // A compile-time constant array of CSS properties.
    // Order must match the StylePropertyID enum order.
   
    namespace {  // anonymous namespace to limit the scope.

        const CssProperty backgroundImage{ StylePropertyID::kBackgroundImage, "background-image", false, false };
        const CssProperty left{ StylePropertyID::kLeft, "left",false, false };
        const CssProperty top{ StylePropertyID::kTop, "top", false, false };
        const CssProperty width{ StylePropertyID::kWidth, "width", false, false };
        const CssProperty height{ StylePropertyID::kHeight, "height", false, false };
        const CssProperty font{ StylePropertyID::kFont, "font", true, false };
        const CssProperty paddingTop{ StylePropertyID::kPaddingTop, "padding-top", false, false };
        const CssProperty paddingLeft{ StylePropertyID::kPaddingLeft, "padding-left", false, false };
        const CssProperty paddingBottom{ StylePropertyID::kPaddingBottom, "padding-bottom", false, false };
        const CssProperty paddingRight{ StylePropertyID::kPaddingRight, "padding-right", false, false };
        const CssPadding padding{ StylePropertyID::kPadding, "padding", false, true };
    }
            
    const std::array<const CssProperty*, static_cast<size_t>(StylePropertyID::kCount)> kCssProperties{ {
        &backgroundImage,
        &left,
        &top,
        &width,
        &height,
        &font,
        &padding,
        &paddingTop,
        &paddingLeft,
        &paddingBottom,
        &paddingRight,
        } };

    std::unordered_map<std::string, StylePropertyID> kCssPropertyNames;

    std::array<const char*, static_cast<size_t>(StylePropertyID::kCount)> kDefaultValues = {
        "none",  // kBackgroundImage default.
        "0",     // kLeft default.
        "0",     // kTop default.
        "auto",  // kWidth default.
        "auto"   // kHeight default.
        "", // font default
        "0", // padding-top default
        "0", // padding-left default
        "0", // padding-bottom default
        "0", // padding-right default
        "0", // padding default
    };

    const std::unordered_map<std::string, const CssProperty*>& CssProperty::GetPropertyLookup()
    {
        static std::unordered_map<std::string, const CssProperty*> lookup;
        if (lookup.empty()) {
            for (const auto& property : kCssProperties) {
                lookup[property->name] = property;
            }
        }
        return lookup;
    }
}