#pragma once

#include <Windows.h>

class EditControl {
public:
	EditControl();
	~EditControl();

	HWND GetHandle();

	bool Create(HWND& handleParent);
	void InitFont();
	void DestroyFont();
	int GetFontSize();
	void SetFontSize(int size);

protected:
	void OnNCCreate(HWND& handle);
	void OnCreate();
	void OnDestroy();

private:
	HWND handle_;

	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam,
		LPARAM lParam);
};