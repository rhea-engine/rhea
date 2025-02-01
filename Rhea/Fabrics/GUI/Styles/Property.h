#ifndef RHEA_GUI_STYLE_PROPERTY_H
#define RHEA_GUI_STYLE_PROPERTY_H

#include <string>
#include <array>
#include <unordered_map>

namespace Rhea
{
	enum class StylePropertyID
	{
		kBackgroundImage,
		kLeft,
		kTop,
		kWidth,
		kHeight,
		kFont,
		kCount
	};

	// Forward declare CssProperty.
	struct CssProperty;

	// Forward declare kCssProperties so it's visible to functions defined in CssProperty.
	extern const std::array<CssProperty, static_cast<size_t>(StylePropertyID::kCount)> kCssProperties;
	extern std::array<const char*, static_cast<size_t>(StylePropertyID::kCount)> kDefaultValues;
	extern std::unordered_map<std::string, StylePropertyID> kCssPropertyNames;

	struct CssProperty
	{
		StylePropertyID id;
		char* name;
		bool inherited;

		// Utility function to look up a property by its ID.
		static const CssProperty* GetCssProperty(StylePropertyID id) {
			size_t index = static_cast<size_t>(id);
			if (index < kCssProperties.size()) {
				return &kCssProperties[index];
			}
			return nullptr;
		}

		// function to return a default value for a given property.
		static inline std::string GetDefaultValue(StylePropertyID id) {
			// For demonstration, we return simple strings. In a real system,
			// these might be more complex types or generated differently.
			switch (id) {
			case StylePropertyID::kBackgroundImage: return "none";
			case StylePropertyID::kLeft:            return "0";
			case StylePropertyID::kTop:             return "0";
			case StylePropertyID::kWidth:           return "auto";
			case StylePropertyID::kHeight:          return "auto";
			default:                              return "";
			}
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

