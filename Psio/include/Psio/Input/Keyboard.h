/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_KEYBOARD_H
#define PSIO_KEYBOARD_H

#include <cstdint>

namespace psio
{
	// the codes are based on
	// https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

    enum Key
    {
        NONE = 0,

        // USB page 07

        CODE_A = 4,
        CODE_B = 5,
        CODE_C = 6,
        CODE_D = 7,
        CODE_E = 8,
        CODE_F = 9,
        CODE_G = 10,
        CODE_H = 11,
        CODE_I = 12,
        CODE_J = 13,
        CODE_K = 14,
        CODE_L = 15,
        CODE_M = 16,
        CODE_N = 17,
        CODE_O = 18,
        CODE_P = 19,
        CODE_Q = 20,
        CODE_R = 21,
        CODE_S = 22,
        CODE_T = 23,
        CODE_U = 24,
        CODE_V = 25,
        CODE_W = 26,
        CODE_X = 27,
        CODE_Y = 28,
        CODE_Z = 29,

        CODE_1 = 30,
        CODE_2 = 31,
        CODE_3 = 32,
        CODE_4 = 33,
        CODE_5 = 34,
        CODE_6 = 35,
        CODE_7 = 36,
        CODE_8 = 37,
        CODE_9 = 38,
        CODE_0 = 39,

        CODE_ENTER = 40,
        CODE_ESC = 41,
        CODE_BACKSPACE = 42,
        CODE_TAB = 43,
        CODE_SPACE = 44,

        CODE_MINUS = 45,
        ODE_EQUALS = 46,
        CODE_LEFTBRACKET = 47,
        CODE_RIGHTBRACKET = 48,
        CODE_BACKSLASH = 49,

        CODE_SEMICOLON = 51,
        CODE_APOSTROPHE = 52,
        CODE_GRAVE = 53,
        CODE_COMMA = 54,
        CODE_PERIOD = 55,
        CODE_SLASH = 56,

        CODE_CAPSLOCK = 57,

        CODE_F1 = 58,
        CODE_F2 = 59,
        CODE_F3 = 60,
        CODE_F4 = 61,
        CODE_F5 = 62,
        CODE_F6 = 63,
        CODE_F7 = 64,
        CODE_F8 = 65,
        CODE_F9 = 66,
        CODE_F10 = 67,
        CODE_F11 = 68,
        CODE_F12 = 69,

        CODE_PRINTSCREEN = 70,
        CODE_SCROLLLOCK = 71,
        CODE_PAUSE = 72,
        CODE_INSERT = 73,
        CODE_HOME = 74,
        CODE_PAGEUP = 75,
        CODE_DELETE = 76,
        CODE_END = 77,
        CODE_PAGEDOWN = 78,
        CODE_RIGHT = 79,
        CODE_LEFT = 80,
        CODE_DOWN = 81,
        CODE_UP = 82,

        CODE_NUMLOCKCLEAR = 83,
        CODE_KP_DIVIDE = 84,
        CODE_KP_MULTIPLY = 85,
        CODE_KP_MINUS = 86,
        CODE_KP_PLUS = 87,
        CODE_KP_ENTER = 88,
        CODE_KP_1 = 89,
        CODE_KP_2 = 90,
        CODE_KP_3 = 91,
        CODE_KP_4 = 92,
        CODE_KP_5 = 93,
        CODE_KP_6 = 94,
        CODE_KP_7 = 95,
        CODE_KP_8 = 96,
        CODE_KP_9 = 97,
        CODE_KP_0 = 98,
        CODE_KP_PERIOD = 99,

        CODE_NONUSBACKSLASH = 100,
        CODE_APPLICATION = 101,
        CODE_KP_EQUALS = 103,
        CODE_F13 = 104,
        CODE_F14 = 105,
        CODE_F15 = 106,
        CODE_F16 = 107,
        CODE_F17 = 108,
        CODE_F18 = 109,
        CODE_F19 = 110,
        CODE_F20 = 111,
        CODE_F21 = 112,
        CODE_F22 = 113,
        CODE_F23 = 114,
        CODE_F24 = 115,
        CODE_EXECUTE = 116,
        CODE_HELP = 117,
        CODE_MENU = 118,
        CODE_SELECT = 119,
        CODE_STOP = 120,
        CODE_AGAIN = 121,
        CODE_UNDO = 122,
        CODE_CUT = 123,
        CODE_COPY = 124,
        CODE_PASTE = 125,
        CODE_FIND = 126,
        CODE_MUTE = 127,
        CODE_VOLUMEUP = 128,
        CODE_VOLUMEDOWN = 129,

        CODE_KP_COMMA = 133,
        CODE_KP_EQUALSAS400 = 134,

        CODE_INTERNATIONAL1 = 135, // used on Asian keyboards

        CODE_INTERNATIONAL2 = 136,
        CODE_INTERNATIONAL3 = 137, // Yen
        CODE_INTERNATIONAL4 = 138,
        CODE_INTERNATIONAL5 = 139,
        CODE_INTERNATIONAL6 = 140,
        CODE_INTERNATIONAL7 = 141,
        CODE_INTERNATIONAL8 = 142,
        CODE_INTERNATIONAL9 = 143,
        CODE_LANG1 = 144,
        CODE_LANG2 = 145,
        CODE_LANG3 = 146,
        CODE_LANG4 = 147,
        CODE_LANG5 = 148,
        CODE_LANG6 = 149,
        CODE_LANG7 = 150,
        CODE_LANG8 = 151,
        CODE_LANG9 = 152,

        CODE_ALTERASE = 153,
        CODE_SYSREQ = 154,
        CODE_CANCEL = 155,
        CODE_CLEAR = 156,
        CODE_PRIOR = 157,
        CODE_RETURN2 = 158,
        CODE_SEPARATOR = 159,
        CODE_OUT = 160,
        CODE_OPER = 161,
        CODE_CLEARAGAIN = 162,
        CODE_CRSEL = 163,
        CODE_EXSEL = 164,

        CODE_KP_00 = 176,
        CODE_KP_000 = 177,
        CODE_THOUSANDSSEPARATOR = 178,
        CODE_DECIMALSEPARATOR = 179,
        CODE_CURRENCYUNIT = 180,
        CODE_CURRENCYSUBUNIT = 181,
        CODE_KP_LEFTPAREN = 182,
        CODE_KP_RIGHTPAREN = 183,
        CODE_KP_LEFTBRACE = 184,
        CODE_KP_RIGHTBRACE = 185,
        CODE_KP_TAB = 186,
        CODE_KP_BACKSPACE = 187,
        CODE_KP_A = 188,
        CODE_KP_B = 189,
        CODE_KP_C = 190,
        CODE_KP_D = 191,
        CODE_KP_E = 192,
        CODE_KP_F = 193,
        CODE_KP_XOR = 194,
        CODE_KP_POWER = 195,
        CODE_KP_PERCENT = 196,
        CODE_KP_LESS = 197,
        CODE_KP_GREATER = 198,
        CODE_KP_AMPERSAND = 199,
        CODE_KP_DBLAMPERSAND = 200,
        CODE_KP_VERTICALBAR = 201,
        CODE_KP_DBLVERTICALBAR = 202,
        CODE_KP_COLON = 203,
        CODE_KP_HASH = 204,
        CODE_KP_SPACE = 205,
        CODE_KP_AT = 206,
        CODE_KP_EXCLAM = 207,
        CODE_KP_MEMSTORE = 208,
        CODE_KP_MEMRECALL = 209,
        CODE_KP_MEMCLEAR = 210,
        CODE_KP_MEMADD = 211,
        CODE_KP_MEMSUBTRACT = 212,
        CODE_KP_MEMMULTIPLY = 213,
        CODE_KP_MEMDIVIDE = 214,
        CODE_KP_PLUSMINUS = 215,
        CODE_KP_CLEAR = 216,
        CODE_KP_CLEARENTRY = 217,
        CODE_KP_BINARY = 218,
        CODE_KP_OCTAL = 219,
        CODE_KP_DECIMAL = 220,
        CODE_KP_HEXADECIMAL = 221,

        CODE_CTRL = 224, // used for both (l+r) 
        CODE_SHIFT = 225, // ...
        CODE_ALT = 226, // ...
        CODE_LGUI = 227,
        CODE_RCTRL = 228, // unused (see above)
        CODE_RSHIFT = 229, // ...
        CODE_RALT = 230, // ...
        CODE_RGUI = 231
    };

    void initHwKey();
    Key hwKey(int code);
}

#endif // !PSIO_KEYBOARD_H
