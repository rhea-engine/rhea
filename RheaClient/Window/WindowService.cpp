#include "RheaClient/Window/WindowService.h"
#include <Psio/Log.h>

namespace rhea::client
{
	WindowId WindowRegistryService::registerWindow(poseidon::Window* renderWindow, const std::string& name)
	{
		if (!renderWindow) {
			LOG(LOG_ERROR) << "WindowRegistryService::registerWindow: renderWindow is null";
			return WindowId();
		}

		// Generate new WindowId
		WindowId id = m_nextId++;

		// Store mapping
		WindowEntry entry;
		entry.id = id;
		entry.renderWindow = renderWindow;
		entry.psioWindow = renderWindow->getNativeWindow();
		entry.name = name;

		m_windows[id] = entry;
		
		// Create reverse lookup
		if (entry.psioWindow) {
			m_psioToId[entry.psioWindow] = id;
		}

		LOG(LOG_DEBUG) << "Registered window '" << name << "' with ID " << id;

		return id;
	}

	void WindowRegistryService::unregisterWindow(WindowId id)
	{
		auto it = m_windows.find(id);
		if (it == m_windows.end()) {
			LOG(LOG_WARNING) << "WindowRegistryService::unregisterWindow: window ID " << id << " not found";
			return;
		}

		// Remove reverse lookup
		if (it->second.psioWindow) {
			m_psioToId.erase(it->second.psioWindow);
		}

		LOG(LOG_DEBUG) << "Unregistered window '" << it->second.name << "' with ID " << id;

		// Remove from main map
		m_windows.erase(it);
	}

	WindowId WindowRegistryService::getWindowId(const psio::Window* window) const
	{
		if (!window)
			return WindowId();

		auto it = m_psioToId.find(window);
		if (it != m_psioToId.end()) {
			return it->second;
		}

		return WindowId();
	}

	psio::Window* WindowRegistryService::getPsioWindow(WindowId id) const
	{
		auto it = m_windows.find(id);
		if (it != m_windows.end()) {
			return it->second.psioWindow;
		}
		return nullptr;
	}

	poseidon::Window* WindowRegistryService::getRenderWindow(WindowId id) const
	{
		auto it = m_windows.find(id);
		if (it != m_windows.end()) {
			return it->second.renderWindow;
		}
		return nullptr;
	}

	std::string WindowRegistryService::getWindowName(WindowId id) const
	{
		auto it = m_windows.find(id);
		if (it != m_windows.end()) {
			return it->second.name;
		}
		return "";
	}

	std::vector<WindowId> WindowRegistryService::getAllWindowIds() const
	{
		std::vector<WindowId> ids;
		ids.reserve(m_windows.size());
		
		for (const auto& pair : m_windows) {
			ids.push_back(pair.first);
		}
		
		return ids;
	}
}