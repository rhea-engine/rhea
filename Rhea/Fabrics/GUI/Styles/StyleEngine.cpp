#include "StyleEngine.h"
#include "StyleSheet.h"

namespace Rhea
{
	void StyleEngine::AppendStylesheet(StyleSheet* sheet)
	{
        m_stylesheets.push_back(sheet);
        
        // fill rules from this sheet

        for (const auto& rule : sheet->getRules()) {
            // TODO Add selector based on full sequence 
            // e.g with: for (const auto& selector : rule.selector) 
            if (rule.selector.size() > 0 && rule.selector[0].sequence.size() > 0) {
                SimpleParser::CssParser::SimpleSelector simple_sel = rule.selector[0].sequence[0];

                if (simple_sel.type == SimpleParser::CssParser::SimpleSelectorType::kClass) {
                    m_class_rules[simple_sel.name] = rule.properties;
                }
                else if (simple_sel.type == SimpleParser::CssParser::SimpleSelectorType::kId) {
                    m_name_rules[simple_sel.name] = rule.properties;
                }
                else if (simple_sel.type == SimpleParser::CssParser::SimpleSelectorType::kType) {
                    m_type_rules[simple_sel.name] = rule.properties;
                }
            }
        }
	}

    // Computes and returns the style properties for a given widget.
    CssRuleSet StyleEngine::GetStylesForWidget(Widget* widget)
    {
        CssRuleSet computed_styles;

        auto type_it = m_type_rules.find(widget->getSelector());
        if (type_it != m_type_rules.end()) {
            // Merge style properties (later rules override earlier ones).
            for (const auto& prop : type_it->second) {
                computed_styles[prop.first] = prop.second;
            }
        }

        // get rules for class names
        auto class_it = m_class_rules.find(widget->getValue("cssclass"));
        if (class_it != m_class_rules.end()) {
            // Merge style properties (later rules override earlier ones).
            for (const auto& prop : class_it->second) {
                computed_styles[prop.first] = prop.second;
            }
        }

        // get rules for element names
        auto name_it = m_name_rules.find(widget->getPathName());
        if (name_it != m_name_rules.end()) {
            // Merge style properties (later rules override earlier ones).
            for (const auto& prop : name_it->second) {
                computed_styles[prop.first] = prop.second;
            }
        }

        return computed_styles;
    }
}