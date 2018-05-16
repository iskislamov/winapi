#include <iostream>

#include "OverlappedWindow.h"
#include "resource.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance,
	LPSTR commandLine, int cmdShow) {

	if (!OverlappedWindow::RegisterClass()) {
		std::wcout << L"Window wasn't registred";
		return 0;
	}
	OverlappedWindow window = OverlappedWindow();
	if (!window.Create()) {
		std::wcout << L"Window wasn't created";
		return 0;
	}
	window.Show(cmdShow);
	HACCEL acceleratorHandle;
	acceleratorHandle =
		LoadAccelerators(instance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	MSG message;
	BOOL getMessageResult = 0;
	while ((getMessageResult = GetMessage(&message, 0, 0, 0)) != 0) {
		if (getMessageResult == -1) {
			return -1;
		}
		if (!TranslateAccelerator(window.GetHandle(), acceleratorHandle,
			&message) &&
			!IsDialogMessage(window.GetDialogHandle(), &message)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	DestroyAcceleratorTable(acceleratorHandle);
	return 0;
}