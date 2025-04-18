#ifndef RHEA_GUI_LABEL
#define RHEA_GUI_LABEL

#include "Widget.h"

namespace Rhea
{
	class TextWidget;

	class Label : public Widget {
		RECLASS(Label, Widget, "//meworla/gui/label")
	public:
		Label(std::string path_name, std::string type_name) :
			Widget(path_name, type_name),			
			m_label_item(nullptr),
			m_templating(true),
			m_clip(true)
		{}

		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// update the text
		void setText(std::string text);

		// recalculate my size
		Vector2 innerSize(Vector2 size_constraints);
		void innerGetMinSize(Vector2& min_size);

		// reset the textWidget
		void onSetPosition();

		// styling
		const std::string getSelector() { return "Label"; }
		void ApplyStyles() override;

		// parse template
		void parseText(const std::string text);
		std::string getText();

		// frame handler
		void onFrame(float delta);

		// get info
		TextWidget* getTextWidget() { return m_label_item; }

		// events

		void ready();

	protected:

		// for template parsing
		bool m_templating;
		struct textElem {
			int type;
			std::string value;
			std::string path;
		};
		std::vector<textElem> m_textElements;

		// cache of the textwidget item
		TextWidget* m_label_item;

		// what happens if text is too large
		bool m_clip;
	};
}

#endif // !RHEA_GUI_LABEL
