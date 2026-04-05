#include <Rhea/Text/TextFabric.h>

#include <Rhea/Text/TextWidget.h>
#include <Rhea/Text/BitmapFont.h>

namespace rhea
{
	void TextFabric::Register(Application* app)
	{
		simpleparser::Type::registerType<TextWidget>("//meworla/textwidget");
		simpleparser::Type::registerType<Font>("//meworla/font");
		simpleparser::Type::registerType<BitmapFont>("//meworla/bitmapfont");
	}
}