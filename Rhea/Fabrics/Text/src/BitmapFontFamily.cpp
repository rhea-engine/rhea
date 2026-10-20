/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Text/BitmapFontFamily.h>
#include <Rhea/Text/Font.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>
#include <cmath>
#include <cstdlib>
#include <iterator>

namespace rhea
{
	void BitmapFontFamily::setup(Application* app,
		std::unordered_map<std::string, std::string> attributes,
		std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);
		m_default_size = getValue("default_size", 0.0f);

		if (m_font_refs.empty()) {
			for (const auto& value : m_values) {
				if (value.first.compare(0, 5, "font_") != 0)
					continue;

				char* end = nullptr;
				const float size = std::strtof(value.first.c_str() + 5, &end);
				if (end && *end == '\0' && size > 0.0f)
					m_font_refs[size] = Ref<Font>(value.second.toString(), m_attributes["basepath"]);
			}
		}
	}

	std::vector<Unit::ResourceReference> BitmapFontFamily::getReferences()
	{
		std::vector<ResourceReference> refs = FontFamily::getReferences();
		for (const auto& font : m_font_refs)
			refs.push_back({ "font_" + std::to_string(font.first), "//meworla/bitmapfont", font.second.id() });
		return refs;
	}

	void BitmapFontFamily::resolveReferences(const std::string& context_root, const std::string& source_root)
	{
		FontFamily::resolveReferences(context_root, source_root);
		const ResourceContext context{ context_root, source_root };
		for (auto& font : m_font_refs)
			normalizeReference(font.second, context);
	}

	void BitmapFontFamily::instanceSetup(Component* source, Component* target, InstantiationContext& context)
	{
		FontFamily::instanceSetup(source, target, context);

		const auto* source_font_family = doCast<BitmapFontFamily>(source);
		auto* target_font_family = doCast<BitmapFontFamily>(target);

		target_font_family->m_font_refs = source_font_family->m_font_refs;
	}

	void BitmapFontFamily::ready()
	{
		Unit::ready();
		m_fonts.clear();
		for (const auto& variant : m_font_refs) {
			Font* font = m_app->getUnitTree()->resolve(variant.second);
			if (font)
				m_fonts[variant.first] = font;
			else
				LOG(LOG_ERROR) << "BitmapFontFamily::ready(): Font '" << variant.second.id() << "' for " << m_pathname << " not found";
		}
	}

	Font* BitmapFontFamily::resolve(float requested_size)
	{
		if (m_fonts.empty())
			ready();
		if (m_fonts.empty())
			return nullptr;
		if (requested_size <= 0.0f)
			requested_size = m_default_size > 0.0f ? m_default_size : m_fonts.begin()->first;

		auto best = m_fonts.begin();
		float best_distance = std::fabs(best->first - requested_size);
		for (auto candidate = std::next(m_fonts.begin()); candidate != m_fonts.end(); ++candidate) {
			const float distance = std::fabs(candidate->first - requested_size);
			// On a tie prefer the larger atlas; downsampling is safer than upsampling.
			if (distance < best_distance || (distance == best_distance && candidate->first > best->first)) {
				best = candidate;
				best_distance = distance;
			}
		}

		best->second->ready();
		return best->second;
	}
}
