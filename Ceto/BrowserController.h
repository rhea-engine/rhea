#pragma once

#include <string>
#include <vector>
#include <functional>
#include <Rhea/Error.h>
#include <Rhea/Application.h>

class BrowserController
{
public:
	using OpenedHandler = std::function<void(const std::string&, bool)>;
	using LoadingHandler = std::function<void(bool)>;
	using ErrorHandler = std::function<void(const rhea::ErrorReport&)>;

	BrowserController(rhea::Application* app) :
		m_app(app)
	{}

	void openScene(std::string scene_name, bool mark_preferences = true);

	void addOpenedHandler(OpenedHandler handler) { m_opened_handlers.push_back(handler); }
	void setLoadingHandler(LoadingHandler handler) { m_loading_handler = handler; }
	void setErrorHandler(ErrorHandler handler) { m_error_handler = handler; }
	
private:
	rhea::Application* m_app = nullptr;

	std::vector<OpenedHandler> m_opened_handlers;
	LoadingHandler m_loading_handler;
	ErrorHandler m_error_handler;
};