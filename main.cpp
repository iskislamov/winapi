#include "COverlappedWindow.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance,
	LPWSTR commandLine, int cmdShow) {
	COverlappedWindow::RegisterClass();
	COverlappedWindow window;
	window.Create();
	window.Show(cmdShow);
	MSG message;
	BOOL gotMessage = 0;
	while ((gotMessage = GetMessage(&message, (HWND)NULL, 0, 0)) != 0 &&
		gotMessage != -1) {
		::DispatchMessage(&message);
	}
	return 0;
}