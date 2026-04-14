#ifndef RHEA_GUI_DIALOG_H
#define RHEA_GUI_DIALOG_H

#include "Widget.h"
#include "Label.h"

namespace rhea
{
	class Dialog : public Widget
	{
		MECLASS(Dialog, Widget)
	public:
		Dialog(std::string path_name) :
			Dialog(path_name, "//meworla/gui/dialog")
		{}
		Dialog(std::string path_name, std::string type_name) :
			Widget(path_name, type_name)		
		{}

		void ready() override;

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void onSetPosition();

		// styling
		const std::string getSelector() { return "Dialog"; }

		bool onKeyPressed(Event* ev, Unit* u) override;

		// dialog specific events
		virtual void cancel();
		virtual void submit();

		// modality
		void setModal(bool modal) { m_is_modal = modal; }
		bool isModal() { return m_is_modal; }

		void show() override;

		// focus handling
		bool isFocusable() const override { return true; }
		bool isTabStop() const { return false; }
		Widget* getInitialFocusWidget() { if (!m_initial_focus_widget) m_initial_focus_widget = findInitialFocusWidget();  return m_initial_focus_widget; }
		void setInitialFocusWidget(Widget* widget) { m_initial_focus_widget = widget; }
		virtual void closeDialog() { hide(); }
		Widget* findInitialFocusWidget();
		// find child that has value defaultButton and set it as default button
		Widget* findDefaultButton() const;

		void setDefaultButton(Widget* button) { m_default_button = button; }
		Widget* getDefaultButton() { return m_default_button; }

	protected:
		bool m_finished = false;
		bool m_is_modal = false;
		Widget* m_initial_focus_widget = nullptr;
		Widget* m_default_button = nullptr;
	};
}

#endif // !RHEA_GUI_DIALOG_H
