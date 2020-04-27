#include <windows.h>
#include "../Keyboard.h"

namespace Psio
{
	Key hw_keycode[256];

	void initHwKey()
	{
		// clear
		for (int i = 0; i < 256; i++)
			hw_keycode[i] = Key::NONE;

		hw_keycode[VK_BACK] = Key::CODE_BACKSPACE;
		hw_keycode[VK_TAB] = Key::CODE_TAB;
		hw_keycode[VK_RETURN] = Key::CODE_ENTER;
		hw_keycode[VK_SHIFT] = Key::CODE_SHIFT;
		hw_keycode[VK_CONTROL] = Key::CODE_CTRL;
		hw_keycode[VK_MENU] = Key::CODE_ALT;
		hw_keycode[VK_PAUSE] = Key::CODE_PAUSE;
		hw_keycode[VK_CAPITAL] = Key::CODE_CAPSLOCK;
		hw_keycode[VK_ESCAPE] = Key::CODE_ESC;
		hw_keycode[VK_SPACE] = Key::CODE_SPACE;
		hw_keycode[VK_PRIOR] = Key::CODE_PRIOR;
		hw_keycode[VK_NEXT] = Key::NONE; // ?
		hw_keycode[VK_END] = Key::CODE_END;
		hw_keycode[VK_HOME] = Key::CODE_HOME;
		hw_keycode[VK_LEFT] = Key::CODE_LEFT;
		hw_keycode[VK_UP] = Key::CODE_UP;
		hw_keycode[VK_RIGHT] = Key::CODE_RIGHT;
		hw_keycode[VK_DOWN] = Key::CODE_DOWN;
		hw_keycode[VK_SELECT] = Key::CODE_SELECT;
		hw_keycode[VK_PRINT] = Key::CODE_PRINTSCREEN;
		hw_keycode[VK_EXECUTE] = Key::CODE_EXECUTE;
		hw_keycode[VK_SNAPSHOT] = Key::NONE; // ?
		hw_keycode[VK_INSERT] = Key::CODE_INSERT;
		hw_keycode[VK_DELETE] = Key::CODE_DELETE;
		hw_keycode[VK_HELP] = Key::CODE_HELP;
		hw_keycode[0x30] = Key::CODE_0; // VK_0, VK_A etc. is not defined in winuser.h
		hw_keycode[0x31] = Key::CODE_1;
		hw_keycode[0x32] = Key::CODE_2;
		hw_keycode[0x33] = Key::CODE_3;
		hw_keycode[0x34] = Key::CODE_4;
		hw_keycode[0x35] = Key::CODE_5;
		hw_keycode[0x36] = Key::CODE_6;
		hw_keycode[0x37] = Key::CODE_7;
		hw_keycode[0x38] = Key::CODE_8;
		hw_keycode[0x39] = Key::CODE_9;
		hw_keycode[0x41] = Key::CODE_A;
		hw_keycode[0x42] = Key::CODE_B;
		hw_keycode[0x43] = Key::CODE_C;
		hw_keycode[0x44] = Key::CODE_D;
		hw_keycode[0x45] = Key::CODE_E;
		hw_keycode[0x46] = Key::CODE_F;
		hw_keycode[0x47] = Key::CODE_G;
		hw_keycode[0x48] = Key::CODE_H;
		hw_keycode[0x49] = Key::CODE_I;
		hw_keycode[0x4A] = Key::CODE_J;
		hw_keycode[0x4B] = Key::CODE_K;
		hw_keycode[0x4C] = Key::CODE_L;
		hw_keycode[0x4D] = Key::CODE_M;
		hw_keycode[0x4E] = Key::CODE_N;
		hw_keycode[0x4F] = Key::CODE_O;
		hw_keycode[0x50] = Key::CODE_P;
		hw_keycode[0x51] = Key::CODE_Q;
		hw_keycode[0x52] = Key::CODE_R;
		hw_keycode[0x53] = Key::CODE_S;
		hw_keycode[0x54] = Key::CODE_T;
		hw_keycode[0x55] = Key::CODE_U;
		hw_keycode[0x56] = Key::CODE_V;
		hw_keycode[0x57] = Key::CODE_W;
		hw_keycode[0x58] = Key::CODE_X;
		hw_keycode[0x59] = Key::CODE_Y;
		hw_keycode[0x5A] = Key::CODE_Z;
		hw_keycode[VK_LWIN] = Key::CODE_LGUI;
		hw_keycode[VK_RWIN] = Key::CODE_RGUI;
		hw_keycode[VK_APPS] = Key::CODE_APPLICATION;
		hw_keycode[VK_NUMPAD0] = Key::CODE_KP_0;
		hw_keycode[VK_NUMPAD1] = Key::CODE_KP_1;
		hw_keycode[VK_NUMPAD2] = Key::CODE_KP_2;
		hw_keycode[VK_NUMPAD3] = Key::CODE_KP_3;
		hw_keycode[VK_NUMPAD4] = Key::CODE_KP_4;
		hw_keycode[VK_NUMPAD5] = Key::CODE_KP_5;
		hw_keycode[VK_NUMPAD6] = Key::CODE_KP_6;
		hw_keycode[VK_NUMPAD7] = Key::CODE_KP_7;
		hw_keycode[VK_NUMPAD8] = Key::CODE_KP_8;
		hw_keycode[VK_NUMPAD9] = Key::CODE_KP_9;
		hw_keycode[VK_MULTIPLY] = Key::CODE_KP_MULTIPLY;
		hw_keycode[VK_ADD] = Key::CODE_KP_MEMADD;
		hw_keycode[VK_SEPARATOR] = Key::CODE_SEPARATOR;
		hw_keycode[VK_SUBTRACT] = Key::CODE_KP_MEMSUBTRACT;
		hw_keycode[VK_DECIMAL] = Key::CODE_KP_DECIMAL;
		hw_keycode[VK_DIVIDE] = Key::CODE_KP_MEMDIVIDE;
		hw_keycode[VK_F1] = Key::CODE_F1;
		hw_keycode[VK_F2] = Key::CODE_F2;
		hw_keycode[VK_F3] = Key::CODE_F3;
		hw_keycode[VK_F4] = Key::CODE_F4;
		hw_keycode[VK_F5] = Key::CODE_F5;
		hw_keycode[VK_F6] = Key::CODE_F6;
		hw_keycode[VK_F7] = Key::CODE_F7;
		hw_keycode[VK_F8] = Key::CODE_F8;
		hw_keycode[VK_F9] = Key::CODE_F9;
		hw_keycode[VK_F10] = Key::CODE_F10;
		hw_keycode[VK_F11] = Key::CODE_F11;
		hw_keycode[VK_F12] = Key::CODE_F12;
		hw_keycode[VK_F13] = Key::CODE_F13;
		hw_keycode[VK_F14] = Key::CODE_F14;
		hw_keycode[VK_F15] = Key::CODE_F15;
		hw_keycode[VK_F16] = Key::CODE_F16;
		hw_keycode[VK_F17] = Key::CODE_F17;
		hw_keycode[VK_F18] = Key::CODE_F18;
		hw_keycode[VK_F19] = Key::CODE_F19;
		hw_keycode[VK_F20] = Key::CODE_F20;
		hw_keycode[VK_F21] = Key::CODE_F21;
		hw_keycode[VK_F22] = Key::CODE_F22;
		hw_keycode[VK_F23] = Key::CODE_F23;
		hw_keycode[VK_F24] = Key::CODE_F24;
	}

	Key hwKey(int code) {
		if (code < 256)
			return hw_keycode[code];
		else
			return Key::NONE;
	}
}