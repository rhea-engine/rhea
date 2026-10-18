/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_BITMAPFONTFAMILY_H
#define RHEA_BITMAPFONTFAMILY_H

#include "FontFamily.h"
#include <map>
#include <unordered_map>

namespace rhea
{
	class BitmapFontFamily : public FontFamily
	{
		MECLASS(BitmapFontFamily, FontFamily)
	public:
		BitmapFontFamily(std::string name, std::string type_name) :
			m_default_size(0.0f),
			FontFamily(name, type_name)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;
		std::vector<Unit::ResourceReference> getReferences() override;		
		void resolveReferences(const std::string& context_root, const std::string& source_root) override;
		void ready() override;
		Font* resolve(float requested_size) override;

	private:
		std::map<float, Ref<Font>> m_font_refs;
		std::map<float, Font*> m_fonts;
		float m_default_size;
	};
}

#endif // !RHEA_BITMAPFONTFAMILY_H
