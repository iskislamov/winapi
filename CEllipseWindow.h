#pragma once

#include <string>
#include <Windows.h>
#include <errors.h>


class CEllipseWindow {
public:
	CEllipseWindow() {
		handle_ = NULL;
		horiz_ = 30;
		vertic_ = 4;
		horiz_offset_ = 0;
		vertic_offset_ = 0;
		size_ = 100;
		colour_ = RGB(rand() % 256, rand() % 256, rand() % 256);
		change_colour_ = FALSE;
	}
	~CEllipseWindow() {}
	static bool RegisterClass();
	bool Create(HWND parent);
	void Show(int cmdShow);
	HWND GetHandle();

protected:
	void OnDestroy();
	void OnCreate();
	void OnNCCreate(const HWND handle);
	void OnTimer();
	void OnPaint();

private:
	HWND handle_;
	int horiz_;
	int vertic_;
	int horiz_offset_;
	int vertic_offset_;
	int size_;
	COLORREF colour_;
	bool change_colour_;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam,
		LPARAM lParam);
	void changeCoords(RECT rect);
};