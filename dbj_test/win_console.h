#pragma once

#include <stdio.h>

#define STRICT 1
#define NO_MIN_MAX
#define WIN32_LEAN_AND_MEANS
#include <Windows.h>

#ifdef _WIN32_WINNT_WIN10
/*
current machine may or may not  be on WIN10 where VT100 ESC codes are
*supposed to be* on by default, Sigh ...

Reuired WIN10 build number is 10586 or greater

to dance with exact win version please proceed here:
https://docs.microsoft.com/en-us/windows/win32/sysinfo/verifying-the-system-version
*/

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#error ENABLE_VIRTUAL_TERMINAL_PROCESSING not found? Try Visual Studio solution re-targeting to the latest SDK.
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	/*
	If your win10 terminal is not showing colors use this

	will not exit the app *only* if app is started in WIN32 CONSOLE
	Example: if running from git bash on win this will exit the app
	if app output is redirected to file, this will also fail.
	*/
	inline void win_enable_vt_100_and_unicode()
	{
		static bool visited = false;
		if (visited)
			return;

		int rez = SetConsoleOutputCP(CP_UTF8 /*65001*/);
		{
			if (rez == 0)
			{
				DBJ_PERROR;
				exit(-1);
			}
		}
		// Set output mode to handle virtual terminal sequences
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE)
		{
			DBJ_PERROR;
			exit(-1);
		}

		DWORD dwMode;
		if (!GetConsoleMode(hOut, &dwMode))
		{
			fprintf(stderr, "\nFile: %s\nLine: %ul\nWhy: %s\n", __FILE__, __LINE__, ", GetConsoleMode() failed");
			fprintf(stderr, "\nPlease re-run in either WIN console %s", " or powershell console\n");
			DBJ_PERROR;
			exit(-1);
			return;
		}

		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(hOut, dwMode))
		{
			DBJ_PERROR;
			exit(-1);
		}
		visited = true;
	}

	/*
	vs standard immensely complex method for enumerating fonts using win32 api in here you need to send
	exact font name ... which actually is easy to find, just open your cmd.exe and go to properties
	*/
	inline BOOL win_set_console_font( LPCWSTR font_name, SHORT font_height_ /*= SHORT(0)*/)
	{
		CONSOLE_FONT_INFOEX font_info;
		font_info.cbSize = sizeof(CONSOLE_FONT_INFOEX);

		HANDLE con_out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (con_out_handle == INVALID_HANDLE_VALUE)
			return false;

		BOOL rez_ = GetCurrentConsoleFontEx(con_out_handle, TRUE, &font_info);

		if (rez_ == 0)
		{
			DBJ_PERROR;
			exit(-1);
		}

		// set the new font name
		(void)memset(font_info.FaceName, 0, LF_FACESIZE);

		errno_t rez = wcscpy_s(
			font_info.FaceName, LF_FACESIZE,font_name
		);

		if (rez != 0) {
			DBJ_PERROR;
			exit(-1);
		}

		// if requested set the new font size
		if (font_height_ > 0)
		{
			// quietly discard the silly sizes
			if ((font_height_ > 7) && (font_height_ < 145))
			{
				font_info.dwFontSize.Y = font_height_;
			}
		}

		rez_ = SetCurrentConsoleFontEx(
			con_out_handle,
			TRUE, /* for the max window size */
			&font_info);

		if (rez_ == 0)
		{
			DBJ_PERROR;
			exit(-1);
		}
		return true;
	}

#ifdef __cplusplus
} // "C"
#endif // __cplusplus

#else
#error Required WIN10 build number has to be 10586 or greater
#endif // _WIN32_WINNT_WIN10