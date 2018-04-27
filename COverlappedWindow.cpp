#include "COverlappedWindow.h"

bool COverlappedWindow::RegisterClass() {
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = windowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	GetModuleHandleEx(0, NULL, &wndclass.hInstance);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = L"OverlappedMenu";
	wndclass.lpszClassName = L"OverlappedWindow";
	wndclass.hIconSm =
		(HICON)LoadImage(wndclass.hInstance, MAKEINTRESOURCE(5), IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	if (!RegisterClassEx(&wndclass)) {
		return FALSE;
	}
	return TRUE;
}

bool COverlappedWindow::Create() {
	HINSTANCE instance;
	GetModuleHandleEx(0, NULL, &instance);

	CreateWindowEx(0, L"OverlappedWindow", L"Window",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, (HWND)NULL,
		(HMENU)NULL, instance, this);

	if (!handle_) {
		return FALSE;
	}

	CEllipseWindow::RegisterClass();
	for (CEllipseWindow& window : windows_) {
		window.Create(handle_);
		window.Show(SW_SHOW);
	}
	return TRUE;
}

void COverlappedWindow::Show(int cmdShow) {
	ShowWindow(handle_, cmdShow);
	UpdateWindow(handle_);
}

void COverlappedWindow::OnDestroy() {
	KillTimer(handle_, 1);
	PostQuitMessage(0);
}

void COverlappedWindow::OnCreate() { SetTimer(handle_, 1, 500, NULL); }

void COverlappedWindow::OnNCCreate(const HWND handle) { handle_ = handle; }

void COverlappedWindow::OnSize() {
	for (int i = 0; i < windows_.size(); ++i) {
		HWND windowHandle = windows_[i].GetHandle();
		RECT rect;
		GetClientRect(handle_, &rect);

		int column = i % columns_;
		int row = i / rows_;
		SetWindowPos(windowHandle, NULL, rect.right * column / columns_,
			rect.bottom * row / rows_, rect.right / columns_,
			rect.bottom / rows_, 0);
	}
}

LRESULT __stdcall COverlappedWindow::windowProc(HWND handle, UINT message,
	WPARAM wParam, LPARAM lParam) {
	COverlappedWindow* window =
		(COverlappedWindow*)GetWindowLongPtr(handle, GWL_USERDATA);
	switch (message) {
	case WM_NCCREATE:
		window = (COverlappedWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
		SetWindowLongPtr(handle, GWL_USERDATA, (LONG_PTR)window);
		window->OnNCCreate(handle);
		return DefWindowProc(handle, message, wParam, lParam);
	case WM_CREATE:
		window->OnCreate();
		return DefWindowProc(handle, message, wParam, lParam);
	case WM_SIZE:
		window->OnSize();
		return 0;
	case WM_DESTROY:
		window->OnDestroy();
		return 0;
	default:
		return DefWindowProc(handle, message, wParam, lParam);
	}
}