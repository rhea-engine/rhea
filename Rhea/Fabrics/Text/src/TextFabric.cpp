#include <Rhea/Text/TextFabric.h>

#include <Rhea/Text/TextWidget.h>
#include <Rhea/Text/BitmapFont.h>

namespace rhea
{
	void TextFabric::Register(Application* app)
	{
		SimpleParser::Type::registerType<TextWidget>("//meworla/textwidget");
		SimpleParser::Type::registerType<Font>("//meworla/font");
		SimpleParser::Type::registerType<BitmapFont>("//meworla/bitmapfont");
	}
}