#include "Win32Clipboard.h"
#include <Windows.h>
#include <Winuser.h>
#include <limits>
#include <stdexcept>
#include <cstring>
#include <string>
#include <string_view>

namespace psio
{
    static std::wstring utf8ToUtf16(std::string_view text);
	static std::string utf16ToUtf8(std::wstring_view text);

    bool Win32Clipboard::setText(std::string_view text)
    {
        // 1. UTF-8 to UTF-16 conversion.
        std::wstring wideText = utf8ToUtf16(text);

        // 2. Allocate memory including null terminator.
        const SIZE_T bytes = (wideText.size() + 1) * sizeof(wchar_t);

        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (!memory)
            return false;

        void* destination = GlobalLock(memory);
        if (!destination)
        {
            GlobalFree(memory);
            return false;
        }

        std::memcpy(destination, wideText.c_str(), bytes);
        GlobalUnlock(memory);

        // 3. Now modify the clipboard.
        if (!OpenClipboard(nullptr))
        {
            GlobalFree(memory);
            return false;
        }

        bool success = false;

        if (EmptyClipboard())
        {
            success = SetClipboardData(CF_UNICODETEXT, memory) != nullptr;
        }

        CloseClipboard();

        // If successful, Windows has taken ownership of the memory.
        if (!success)
            GlobalFree(memory);

        return success;
    }

    bool Win32Clipboard::getText(std::string& text)
    {
        if (!OpenClipboard(nullptr))
            return false;
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (!hData)
        {
            CloseClipboard();
            return false;
        }
        wchar_t* wideText = static_cast<wchar_t*>(GlobalLock(hData));
        if (!wideText)
        {
            CloseClipboard();
            return false;
        }
        // Convert UTF-16 to UTF-8.
        text = utf16ToUtf8(wideText);
        GlobalUnlock(hData);
        CloseClipboard();
        return true;
    }

    static std::wstring utf8ToUtf16(std::string_view text)
    {
        if (text.empty())
            return {};

        if (text.size() > static_cast<size_t>(
            (std::numeric_limits<int>::max)()))
        {
            throw std::length_error("Text is too long");
        }

        const int inputLength = static_cast<int>(text.size());

        // Benötigte Anzahl UTF-16-Codeeinheiten bestimmen.
        const int length = MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS,
            text.data(), inputLength,
            nullptr, 0);

        if (length == 0)
            throw std::runtime_error("Invalid UTF-8 text");

        std::wstring result(length, L'\0');

        if (MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS,
            text.data(), inputLength,
            result.data(), length) == 0)
        {
            throw std::runtime_error("UTF-8 conversion failed");
        }

        return result;
    }

    static std::string utf16ToUtf8(std::wstring_view text)
    {
        if (text.empty())
            return {};

        if (text.size() > static_cast<size_t>(
            (std::numeric_limits<int>::max)()))
        {
            throw std::length_error("Text is too long");
        }

        const int inputLength = static_cast<int>(text.size());

        // Benötigte Anzahl UTF-8-Codeeinheiten bestimmen.
        const int length = WideCharToMultiByte(
            CP_UTF8, WC_ERR_INVALID_CHARS,
            text.data(), inputLength,
            nullptr, 0,
            nullptr, nullptr);

        if (length == 0)
            throw std::runtime_error("Invalid UTF-16 text");

        std::string result(length, '\0');

        if (WideCharToMultiByte(
            CP_UTF8, WC_ERR_INVALID_CHARS,
            text.data(), inputLength,
            result.data(), length,
            nullptr, nullptr) == 0)
        {
            throw std::runtime_error("UTF-16 conversion failed");
        }

        return result;
    }

    bool setText(std::string_view text) {
        // 1. UTF-8 to UTF-16 conversion.
        std::wstring wideText = utf8ToUtf16(text);

        // 2. Allocate memory including null terminator.
        const SIZE_T bytes = (wideText.size() + 1) * sizeof(wchar_t);

        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (!memory)
            return false;

        void* destination = GlobalLock(memory);
        if (!destination)
        {
            GlobalFree(memory);
            return false;
        }

        std::memcpy(destination, wideText.c_str(), bytes);
        GlobalUnlock(memory);

        // 3. Now modify the clipboard.
        if (!OpenClipboard(nullptr))
        {
            GlobalFree(memory);
            return false;
        }

        bool success = false;

        if (EmptyClipboard())
        {
            success = SetClipboardData(CF_UNICODETEXT, memory) != nullptr;
        }

        CloseClipboard();

        // If successful, Windows has taken ownership of the memory.
        if (!success)
            GlobalFree(memory);

        return success;
    }

	bool getText(std::string& text) {

		if (!OpenClipboard(nullptr))
			return false;
		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		if (!hData)
		{
			CloseClipboard();
			return false;
		}
		wchar_t* wideText = static_cast<wchar_t*>(GlobalLock(hData));
		if (!wideText)
		{
			CloseClipboard();
			return false;
		}
		// Convert UTF-16 to UTF-8.
		text = utf16ToUtf8(wideText);
		GlobalUnlock(hData);
		CloseClipboard();
		return true;
	}
}