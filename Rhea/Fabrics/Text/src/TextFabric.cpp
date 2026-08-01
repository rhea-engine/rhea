/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Text/TextFabric.h>

#include <Rhea/Text/TextWidget.h>
#include <Rhea/Text/BitmapFont.h>
#include <Rhea/Text/FontFamily.h>
#include <Rhea/Text/BitmapFontFamily.h>

namespace rhea
{
	void TextFabric::Register(Application* app)
	{
		simpleparser::Type::registerType<TextWidget>("//meworla/textwidget");
		simpleparser::Type::registerType<Font>("//meworla/font");
		simpleparser::Type::registerType<BitmapFont>("//meworla/bitmapfont");
		simpleparser::Type::registerType<FontFamily>("//meworla/fontfamily");
		simpleparser::Type::registerType<BitmapFontFamily>("//meworla/bitmapfontfamily");
	}
}
