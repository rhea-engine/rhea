#ifndef PSIO_INPUTEVENT_H
#define PSIO_INPUTEVENT_H

#include <memory>
#include <cstdint>
#include <string>

namespace psio
{
	enum class InputEventId {
		NONE,
		KEY_PRESSED,
		KEY_RELEASED,
		TEXT,
		MOUSE_MOVE,
		MOUSE_BTN_PRESSED,
		MOUSE_BTN_RELEASED,
		MOUSE_BTN_DBLCLICK,
		WMGR_QUIT,
		WIN_SIZE
	};

	class Window;

	class InputEvent
	{
	public:
		InputEvent(InputEventId id) :
			m_id(id),
			m_shift(false),
			m_alt(false),
			m_ctrl(false),
			m_hash(0),
			m_canceled(false)
		{}
		virtual ~InputEvent() = default;

		// factory
		static std::shared_ptr<InputEvent> createEvent(InputEventId id);

		// check if the event does match this event
		virtual bool isEvent(InputEvent* evt) { return evt->m_id == m_id; }

		// set the modifiers
		void setModifiers(bool shift, bool alt, bool ctrl);
		// get modifiers
		bool isShift() const { return m_shift; }
		bool isAlt() const { return m_alt; }
		bool isCtrl() const { return m_ctrl; }

		// get id
		InputEventId getId() const { return m_id; }

		// canceling
		bool isCanceled() const { return m_canceled; }
		void cancel() { m_canceled = true; }

		// helper for hashing the input events
		virtual void updateHash(int raw);
		unsigned long hash() { if (!m_hash) updateHash(0); return m_hash; }
		
	protected:
		InputEventId m_id;

		unsigned long m_hash;

		// canceled?
		bool m_canceled;

		// modifiers
		bool m_shift;
		bool m_alt;
		bool m_ctrl;
	};

	class KeyInputEvent : public InputEvent
	{
	public:
		KeyInputEvent(InputEventId id) :
			m_keycode(0),
			m_physical_keycode(0),
			m_unicode(0),
			InputEvent(id)
		{}

		bool isEvent(InputEvent* evt);
		void updateHash(int raw);

		void set(uint32_t keycode, uint32_t physical_keycode, uint8_t unicode);

		uint8_t getCode() const { return m_unicode; }
		uint32_t getKeyCode() const { return m_keycode; }

	protected:
		uint32_t m_keycode;
		uint32_t m_physical_keycode;
		uint8_t m_unicode;
	};

	class TextInputEvent : public InputEvent
	{
	public:
		explicit TextInputEvent(const std::string& utf8Text) :
			InputEvent(InputEventId::TEXT), m_text(utf8Text)
		{
		}

		bool isEvent(InputEvent* evt);

		void setText(const std::string& utf8Text) { m_text = utf8Text; }
		const std::string& getText() const { return m_text; }

	private:
		std::string m_text;
	};

	class MouseInputEvent : public InputEvent
	{
	public:
		MouseInputEvent(InputEventId id) :
			m_left(false),
			m_right(false),
			m_middle(false),
			m_y(0),
			m_x(0),
			InputEvent(id)
		{}

		bool isEvent(InputEvent* evt);
		void updateHash(int raw);

		void setBtn(bool left, bool right, bool middle);
		void setPos(int x, int y);

		int getX() const { return m_x; }
		int getY() const { return m_y; }

		bool isLeft() const { return m_left; }
		bool isRight() const { return m_right; }
		bool isMiddle() const { return m_middle; }

	protected:
		bool m_left;
		bool m_right;
		bool m_middle;
		int m_x;
		int m_y;
	};

	class WinSizeEvent : public InputEvent
	{
	public:
		WinSizeEvent(InputEventId id, unsigned int width, unsigned int height, Window* win) :
			m_width(width),
			m_height(height),
			m_window(win),
			InputEvent(id)
		{}
		unsigned int width() { return m_width; }
		unsigned int height() { return m_height; }
		// get the window that was resized
		Window* getWindow() { return m_window; }

	protected:
		unsigned int m_height;
		unsigned int m_width;
		Window* m_window;
	};
}

#endif // !PSIO_INPUTEVENT_H
