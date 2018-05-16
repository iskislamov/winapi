#pragma once

#include "EditControl.h"
#include "Settings.h"

class OverlappedWindow {
public:
	OverlappedWindow();
	~OverlappedWindow();

	HWND GetHandle();
	HWND GetDialogHandle();

	static bool RegisterClass();

	bool Create();
	void Show(int cmdShow);
	void SetFontColour(COLORREF colour);
	void SetBackgroundColour(COLORREF colour);

	COLORREF GetFontColour();
	COLORREF GetBackgroundColour();

protected:

	void OnCreate();
	void OnNCCreate(HWND& handle);
	void OnSize();
	void OnTextChange();
	void OnClose();
	void OnDestroy();

	LRESULT OnControlColourEdit(HDC hdc);

private:
	HWND handle_;

	EditControl editControl_;

	Setting setting_;

	COLORREF fontColour_;
	COLORREF backgroundColour_;

	bool empty_;

	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam,
		LPARAM lParam);

	void saveFile();
	void showSettings();
};