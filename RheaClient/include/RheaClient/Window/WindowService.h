#ifndef RHEACLIENT_WINDOWSERVICE_H
#define RHEACLIENT_WINDOWSERVICE_H

#include <Rhea/Service.h>
#include <Poseidon/Window.h>
#include <Psio/Window/Window.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace rhea::client
{
	// Type-safe window identifier
	using WindowId = uint32_t;

	/**
	 * @brief Service that manages the mapping between psio::Window and poseidon::Window
	 * 
	 * This service provides a centralized registry for all windows in the application,
	 * allowing bidirectional lookup between platform windows (psio::Window) and 
	 * render windows (poseidon::Window).
	 */
	class WindowRegistryService : public rhea::Service
	{
		RECLASS(WindowRegistryService, Service, "//meworla/windowregistry")
	public:
		WindowRegistryService(std::string name, std::string type_name) :
			m_nextId(1),
			Service(name, type_name)
		{}

		/**
		 * @brief Register a new window pair
		 * @param renderWindow The Poseidon render window
		 * @param name Optional name for the window
		 * @return Unique window identifier
		 */
		WindowId registerWindow(poseidon::Window* renderWindow, const std::string& name = "");

		/**
		 * @brief Unregister a window by ID
		 * @param id The window identifier to unregister
		 */
		void unregisterWindow(WindowId id);

		/**
		 * @brief Get window ID from psio::Window pointer
		 * @param window The platform window
		 * @return WindowId or 0 if not found
		 */
		WindowId getWindowId(const psio::Window* window) const;

		/**
		 * @brief Get psio::Window from window ID
		 * @param id The window identifier
		 * @return Platform window pointer or nullptr if not found
		 */
		psio::Window* getPsioWindow(WindowId id) const;

		/**
		 * @brief Get poseidon::Window from window ID
		 * @param id The window identifier
		 * @return Render window pointer or nullptr if not found
		 */
		poseidon::Window* getRenderWindow(WindowId id) const;

		/**
		 * @brief Get window name
		 * @param id The window identifier
		 * @return Window name or empty string if not found
		 */
		std::string getWindowName(WindowId id) const;

		/**
		 * @brief Get all registered window IDs
		 * @return Vector of all window IDs
		 */
		std::vector<WindowId> getAllWindowIds() const;

	private:
		struct WindowEntry
		{
			WindowId id;
			poseidon::Window* renderWindow;
			psio::Window* psioWindow;
			std::string name;
		};

		WindowId m_nextId;
		std::unordered_map<WindowId, WindowEntry> m_windows;
		std::unordered_map<const psio::Window*, WindowId> m_psioToId;
	};
}

#endif // RHEACLIENT_WINDOWSERVICE_H
