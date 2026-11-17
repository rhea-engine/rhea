#pragma once

#include <Rhea/GUI/Widget.h>

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
};