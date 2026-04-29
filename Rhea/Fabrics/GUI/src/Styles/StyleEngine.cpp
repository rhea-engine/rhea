/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/Styles/StyleEngine.h>
#include <Rhea/GUI/Styles/StyleSheet.h>
#include <Rhea/GUI/Widget.h>

namespace rhea
{
    void StyleEngine::AppendStylesheet(StyleSheet* sheet)
    {
        m_stylesheets.push_back(sheet);
        
        // fill rules from this sheet

        for (const auto& rule : sheet->getRules()) {
            if (rule.selector.size() > 0 && rule.selector[0].sequence.size() > 0) {
                const auto& sequence = rule.selector[0].sequence;
                
                // Finde Base-Selektor und Pseudo-Klasse
                std::string base_name;
                std::string pseudo_class;
                simpleparser::CssParser::SimpleSelectorType base_type = 
                    simpleparser::CssParser::SimpleSelectorType::kType;
                
                for (const auto& simple_sel : sequence) {
                    if (simple_sel.type == simpleparser::CssParser::SimpleSelectorType::kPseudo) {
                        pseudo_class = simple_sel.name;
                    } else {
                        base_name = simple_sel.name;
                        base_type = simple_sel.type;
                    }
                }
                
                SelectorKey key{ base_name, pseudo_class };
                
                if (base_type == simpleparser::CssParser::SimpleSelectorType::kClass) {
                    m_class_rules[key] = rule.properties;
                }
                else if (base_type == simpleparser::CssParser::SimpleSelectorType::kId) {
                    m_name_rules[key] = rule.properties;
                }
                else if (base_type == simpleparser::CssParser::SimpleSelectorType::kType) {
                    m_type_rules[key] = rule.properties;
                }
            }
        }
    }

    bool StyleEngine::matchesPseudoClass(const std::string& pseudo, Widget* widget) const
    {
        if (pseudo.empty()) {
            return true;  // Keine Pseudo-Klasse = immer matchen
        }
        
        if (pseudo == "hover") {
            return widget->hasWidgetState(Widget::WidgetState::kHover);
        }
        else if (pseudo == "active") {
            return widget->hasWidgetState(Widget::WidgetState::kActive);
        }
        else if (pseudo == "focus") {
            return widget->hasWidgetState(Widget::WidgetState::kFocus);
        }
        else if (pseudo == "disabled") {
            return widget->hasWidgetState(Widget::WidgetState::kDisabled);
        }
        
        return false;
    }

    CssRuleSet StyleEngine::GetStylesForWidget(Widget* widget)
    {
        CssRuleSet computed_styles;
        
        auto applyRules = [&](const std::string& selector_name, 
                             const std::unordered_map<SelectorKey, CssRuleSet, SelectorKeyHash>& rule_map) {
            // Basis-Regel ohne Pseudo-Klasse
            auto it = rule_map.find(SelectorKey{ selector_name, "" });
            if (it != rule_map.end()) {
                for (const auto& prop : it->second) {
                    computed_styles[prop.first] = prop.second;
                }
            }
            
            // Pseudo-Klassen-Regeln
            for (const auto& [key, properties] : rule_map) {
                if (key.base_selector == selector_name && 
                    !key.pseudo_class.empty() && 
                    matchesPseudoClass(key.pseudo_class, widget)) {
                    for (const auto& prop : properties) {
                        computed_styles[prop.first] = prop.second;
                    }
                }
            }
        };
        
        // Typ-Regeln
        applyRules(widget->getSelector(), m_type_rules);
        
        // Klassen-Regeln
        applyRules(widget->getValueString("cssclass"), m_class_rules);
        
        // Name-Regeln
        applyRules(widget->getPathName(), m_name_rules);
        
        return computed_styles;
    }

    bool StyleEngine::HasPseudoClassStyles(Widget* widget, const std::string& pseudo_class) const
    {
        auto checkRules = [&](const std::string& selector_name, 
                             const std::unordered_map<SelectorKey, CssRuleSet, SelectorKeyHash>& rule_map) -> bool {
            SelectorKey key{ selector_name, pseudo_class };
            return rule_map.find(key) != rule_map.end();
        };
        
        // Prüfe alle drei Regel-Typen
        if (checkRules(widget->getSelector(), m_type_rules)) return true;
        if (checkRules(widget->getValueString("cssclass"), m_class_rules)) return true;
        if (checkRules(widget->getPathName(), m_name_rules)) return true;
        
        return false;
    }
}