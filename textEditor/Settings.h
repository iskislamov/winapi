#pragma once

#include "EditControl.h"

class Setting {
public:
	Setting();
	~Setting();

	HWND GetHandle();

	bool Create(HWND& windowHandle);
	void SetEditControl(EditControl& _editControl);

protected:
	void OnInitDialog(HWND& _handle);
	void OnCheckPreview();
	void OnFontChange();
	void OnTransparencyChange();
	void OnFontColourChange();
	void OnBackgroundColourChange();
	void OnOk();
	void OnCancel();

private:
	HWND handle_;

	EditControl editControl_;

	bool preview_;
	int oldFontSize_;
	int fontSize_;
	int oldTransparency_;
	int transparency_;

	COLORREF oldFontColour_;
	COLORREF fontColour_;
	COLORREF oldBackgroundColour_;
	COLORREF backgroundColour_;
	COLORREF colours_[16];

	static INT_PTR __stdcall dialogProc(HWND handle, UINT message, WPARAM wParam,
		LPARAM lParam);
};