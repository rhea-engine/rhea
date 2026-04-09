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

		class FinishedEvent : public Event {
			public:
			enum class FinishType {
				Submit,
				Cancel
			} finishType;

			FinishedEvent(Unit* target, FinishType type) : Event("finished", target), finishType(type) {}
		};

		void ready() override;

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void onSetPosition();

		// styling
		const std::string getSelector() { return "Dialog"; }

		// dialog specific events
		virtual void cancel();
		virtual void submit();

		void show() override;

		virtual void closeDialog() { hide(); }

	protected:
		bool m_finished = false;
	};
}

#endif // !RHEA_GUI_DIALOG_H
