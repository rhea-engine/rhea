/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/Styles/Property.h>
#include <Rhea/GUI/Styles/CssShorthand.h>

namespace rhea
{
    // A compile-time constant array of CSS properties.
    // Order must match the StylePropertyId enum order.
   
    namespace {  // anonymous namespace to limit the scope.

        const CssProperty backgroundImage{ StylePropertyId::kBackgroundImage, "background-image", false, false };
        const CssProperty backgroundColor{ StylePropertyId::kBackgroundColor, "background-color", false, false };
        const CssProperty backgroundSize{ StylePropertyId::kBackgroundSize, "background-size", false, false };
        const CssProperty left{ StylePropertyId::kLeft, "left",false, false };
        const CssProperty top{ StylePropertyId::kTop, "top", false, false };
        const CssProperty width{ StylePropertyId::kWidth, "width", false, false };
        const CssProperty height{ StylePropertyId::kHeight, "height", false, false };
        const CssProperty font{ StylePropertyId::kFont, "font", true, false };
        const CssProperty fontSize{ StylePropertyId::kFont, "font-size", true, false };
		const CssProperty overflow{ StylePropertyId::kOverflow, "overflow", false, false };
		const CssProperty alignItems{ StylePropertyId::kAlignItems, "align-items", false, false };
		const CssProperty justifyContent{ StylePropertyId::kJustifyContent, "justify-content", false, false };
        const CssProperty paddingTop{ StylePropertyId::kPaddingTop, "padding-top", false, false };
        const CssProperty paddingLeft{ StylePropertyId::kPaddingLeft, "padding-left", false, false };
        const CssProperty paddingBottom{ StylePropertyId::kPaddingBottom, "padding-bottom", false, false };
        const CssProperty paddingRight{ StylePropertyId::kPaddingRight, "padding-right", false, false };
        const CssPadding padding{ StylePropertyId::kPadding, "padding", false, true };
    }
            
    const std::array<const CssProperty*, static_cast<size_t>(StylePropertyId::kCount)> kCssProperties{ {
        &backgroundImage,
        &backgroundColor,
        &backgroundSize,
        &left,
        &top,
        &width,
        &height,
        &font,
        &fontSize,
        &overflow,
        &alignItems,
        &justifyContent,
        &padding,
        &paddingTop,
        &paddingLeft,
        &paddingBottom,
        &paddingRight,
        } };

    std::unordered_map<std::string, StylePropertyId> kCssPropertyNames;

    std::array<_css_def, static_cast<size_t>(StylePropertyId::kCount)> kDefaultValues = { {
        {CssValueId::kIdentifier, "none"},  // kBackgroundImage default.
        {CssValueId::kIdentifier, "transparent"},  // kBackgroundColor default.
        {CssValueId::kIdentifier, "auto"}, // kBackgroundSize default
        {CssValueId::kNumeric, "0"},     // kLeft default.
        {CssValueId::kNumeric, "0"},     // kTop default.
        {CssValueId::kIdentifier, "auto"},  // kWidth default.
        {CssValueId::kIdentifier,"auto"},   // kHeight default.
		{CssValueId::kString, "visible"}, // kOverflow default
		{CssValueId::kIdentifier, "normal"}, // kAlignItems default
		{CssValueId::kIdentifier, "normal"}, // kJustifyItems default
        {CssValueId::kString, ""}, // font default
        {CssValueId::kNumeric, "16"}, // font-size default
        {CssValueId::kNumeric, "0"}, // padding-top default
        {CssValueId::kNumeric, "0"}, // padding-left default
        {CssValueId::kNumeric, "0"}, // padding-bottom default
        {CssValueId::kNumeric, "0"}, // padding-right default
        {CssValueId::kNumeric, "0"}, // padding default
    } };

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