#pragma once

#include <Rhea/GUI/Widget.h>
#include <Rhea/GUI/Button.h>
#include <unordered_map>
#include <string>

class ViewerSession;

class ViewerPanel : public rhea::Widget
{
	RECLASS(ViewerPanel, rhea::Widget, "//ceto/gui/viewerpanel")
public:
	ViewerPanel(std::string name, std::string type_name) :
		rhea::Widget(name, type_name)
	{
		defineSlot("content", false);
	}

	void ready() override;

	void onChange();

	void bindViewerSession(ViewerSession* session) { m_session = session; }

private:
	rhea::Widget* m_playlist = nullptr;
	ViewerSession* m_session = nullptr;
	struct play_state {
		std::string state_name;
		bool is_playing = false;
		rhea::Button* play_button = nullptr;
	};
	std::unordered_map<rhea::Component*, play_state> m_current_animation_targets;
};