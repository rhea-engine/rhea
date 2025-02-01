#include "StyleEngine.h"
#include "StyleSheet.h"

namespace Rhea
{
	void StyleEngine::AppendStylesheet(StyleSheet* sheet)
	{
        m_stylesheets.push_back(sheet);
	}

    // Computes and returns the style properties for a given widget.
    std::unordered_map<StylePropertyID, SimpleParser::Variant> StyleEngine::GetStylesForWidget(Widget* widget)
    {
        std::unordered_map<StylePropertyID, SimpleParser::Variant> computedStyles;

        // Iterate over all style sheets.
        for (auto sheet : m_stylesheets) {
            for (const auto& rule : sheet->getRules()) {
                // For this example, we assume the selector is a class name.
                // If the selector starts with a '.', we remove it.
                std::string ruleClass = rule.selector;
                if (!ruleClass.empty() && ruleClass[0] == '.')
                    ruleClass = ruleClass.substr(1);

                // Check if the widget's classname matches the rule's selector.
                if (ruleClass == widget->getSelector()) {
                    // Merge style properties (later rules override earlier ones).
                    for (const auto& prop : rule.properties) {
                        computedStyles[prop.first] = prop.second;
                    }
                }
            }
        }
        return computedStyles;
    }
}