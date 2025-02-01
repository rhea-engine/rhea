#include "Property.h"

namespace Rhea
{
    // A compile-time constant array of CSS properties.
    // Order must match the StylePropertyID enum order.
    const std::array<CssProperty, static_cast<size_t>(StylePropertyID::kCount)> kCssProperties{ {
        { StylePropertyID::kBackgroundImage, "background-image", false },
        { StylePropertyID::kLeft,            "left",            false },
        { StylePropertyID::kTop,             "top",             false },
        { StylePropertyID::kWidth,           "width",           false },
        { StylePropertyID::kHeight,          "height",          false },
        { StylePropertyID::kFont,            "font",            true },
    } };

    std::unordered_map<std::string, StylePropertyID> kCssPropertyNames;

    std::array<const char*, static_cast<size_t>(StylePropertyID::kCount)> kDefaultValues = {
    "none",  // kBackgroundImage default.
    "0",     // kLeft default.
    "0",     // kTop default.
    "auto",  // kWidth default.
    "auto"   // kHeight default.
    ""
    };

    const std::unordered_map<std::string, const CssProperty*>& CssProperty::GetPropertyLookup()
    {
        static std::unordered_map<std::string, const CssProperty*> lookup;
        if (lookup.empty()) {
            for (const auto& property : kCssProperties) {
                lookup[property.name] = &property;
            }
        }
        return lookup;
    }
}