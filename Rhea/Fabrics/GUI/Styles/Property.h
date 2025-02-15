#ifndef RHEA_GUI_STYLE_PROPERTY_H
#define RHEA_GUI_STYLE_PROPERTY_H

#include <string>
#include <array>
#include <unordered_map>

#include "CssValue.h"

namespace Rhea
{
	enum class StylePropertyId
	{
		kBackgroundImage,
		kBackgroundColor,
		kLeft,
		kTop,
		kWidth,
		kHeight,
		kFont,
		kFontSize,
		kPaddingTop,
		kPaddingLeft,
		kPaddingBottom,
		kPaddingRight,
		kPadding,
		kCount
	};

	// Forward declare CssProperty.
	struct CssProperty;
	// Forward declare CssValueId

	struct _css_def {
		CssValueId id;
		SimpleParser::Variant value;
	};

	// Forward declare kCssProperties so it's visible to functions defined in CssProperty.
	extern const std::array<const CssProperty*, static_cast<size_t>(StylePropertyId::kCount)> kCssProperties;
	extern std::array<_css_def, static_cast<size_t>(StylePropertyId::kCount)> kDefaultValues;
	extern std::unordered_map<std::string, StylePropertyId> kCssPropertyNames;

	struct CssProperty
	{
		StylePropertyId id;
		char* name;
		bool inherited;
		bool shorthand;

		CssProperty(StylePropertyId id, char* name, bool inherited, bool shorthand) :
			id(id),
			name(name),
			inherited(inherited),
			shorthand(shorthand)
		{}

		// Utility function to look up a property by its ID.
		static const CssProperty* GetCssProperty(StylePropertyId id) {
			size_t index = static_cast<size_t>(id);
			if (index < kCssProperties.size()) {
				return kCssProperties[index];
			}
			return nullptr;
		}

		// function to return a default value for a given property.
		static inline CssValue GetDefaultValue(StylePropertyId id) {
			size_t index = static_cast<size_t>(id);
			if (index < kCssProperties.size()) {
				CssValue val = { kDefaultValues[index].id, kDefaultValues[index].value };
				return val;
			}
			CssValue val = { CssValueId::kString, "" };
			return val;
		}

		// Build a static lookup map from property name to property metadata.
		static const std::unordered_map<std::string, const CssProperty*>& GetPropertyLookup();

		// Lookup function using the unordered_map.
		static const CssProperty* FindCssProperty(std::string name) {
			const auto& lookup = GetPropertyLookup();
			auto it = lookup.find(name);
			return (it != lookup.end()) ? it->second : nullptr;
		}

	};
}

#endif // !RHEA_GUI_STYLE_PROPERTY_H

