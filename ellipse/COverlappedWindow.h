#pragma once

#include "CEllipseWindow.h"
#include <string>
#include <errors.h>
#include <vector>

class COverlappedWindow {
public:
	COverlappedWindow() {
		handle_ = NULL;
		rows_ = 2;
		columns_ = 2;
		windows_.resize(rows_ * columns_);
	}
	~COverlappedWindow() {}

	static bool RegisterClass();
	bool Create();
	void Show(int cmdShow);

protected:
	void OnDestroy();
	void OnCreate();
	void OnNCCreate(const HWND handle);
	void OnSize();

private:
	HWND handle_;
	std::vector<CEllipseWindow> windows_;
	int rows_;
	int columns_;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam,
		LPARAM lParam);
};