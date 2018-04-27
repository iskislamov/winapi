#include "CEllipseWindow.h"

CONST int NUMBER = 107;
CONST int OFFSET = 256 - NUMBER;

bool CEllipseWindow::RegisterClass() {
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
	wndclass.lpszMenuName = L"EllipseMenu";
	wndclass.lpszClassName = L"CEllipseWindow";
	wndclass.hIconSm =
		(HICON)LoadImage(wndclass.hInstance, MAKEINTRESOURCE(5), IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	if (!RegisterClassEx(&wndclass)) {
		return FALSE;
	}
	return TRUE;
}

bool CEllipseWindow::Create(HWND parent) {
	HINSTANCE instance;
	GetModuleHandleEx(0, NULL, &instance);

	CreateWindowEx(0, L"CEllipseWindow", L"Window", WS_CHILD | WS_BORDER,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		parent, (HMENU)NULL, instance, this);

	if (!handle_) {
		return FALSE;
	}

	return TRUE;
}

void CEllipseWindow::Show(int cmdShow) {
	ShowWindow(handle_, cmdShow);
	UpdateWindow(handle_);
}

HWND CEllipseWindow::GetHandle() { return this->handle_; }

void CEllipseWindow::OnDestroy() {
	KillTimer(handle_, 1);
	PostQuitMessage(0);
}

void CEllipseWindow::OnCreate() { SetTimer(handle_, 1, 500, NULL); }

void CEllipseWindow::OnNCCreate(const HWND handle) { handle_ = handle; }

void CEllipseWindow::OnTimer() {
	RECT rect;
	GetClientRect(handle_, &rect);
	if (GetFocus() != handle_) {
		change_colour_ = TRUE;
	}
	else {
		if (change_colour_) {
			colour_ = RGB(rand() % NUMBER + OFFSET, rand() % NUMBER + OFFSET,
				rand() % NUMBER + OFFSET);
			change_colour_ = FALSE;
		}
		changeCoords(rect);
		InvalidateRect(handle_, &rect, FALSE);
	}
}

void CEllipseWindow::changeCoords(RECT rect) {
	if (rect.top + size_ + vertic_offset_ > rect.bottom ||
		rect.top + vertic_offset_ < rect.top) {
		vertic_ *= -1;
	}
	if (rect.left + 2 * size_ + horiz_offset_ > rect.right ||
		rect.left + horiz_offset_ < rect.left) {
		horiz_ *= -1;
	}
	horiz_offset_ += horiz_;
	vertic_offset_ += vertic_;
}

void CEllipseWindow::OnPaint() {
	PAINTSTRUCT paintStruct;
	HDC hdc = BeginPaint(handle_, &paintStruct);

	RECT rect;
	GetClientRect(handle_, &rect);

	HDC paintHDC = CreateCompatibleDC(hdc);

	HBITMAP map, prev_map;
	map = CreateCompatibleBitmap(hdc, rect.right - rect.left,
		rect.bottom - rect.top);
	prev_map = (HBITMAP)SelectObject(paintHDC, map);

	HBRUSH ellipse, prev;
	ellipse = CreateSolidBrush(colour_);
	prev = (HBRUSH)SelectObject(paintHDC, ellipse);

	Ellipse(paintHDC, rect.left + horiz_offset_, rect.top + vertic_offset_,
		rect.left + horiz_offset_ + 2 * size_,
		rect.top + vertic_offset_ + size_);

	BitBlt(hdc, rect.left, rect.top, rect.right - rect.left,
		rect.bottom - rect.top, paintHDC, 0, 0, SRCCOPY);

	SelectObject(paintHDC, prev_map);
	DeleteObject(map);
	DeleteObject(ellipse);
	DeleteDC(paintHDC);
	EndPaint(handle_, &paintStruct);
}

LRESULT __stdcall CEllipseWindow::windowProc(HWND handle, UINT message,
	WPARAM wParam, LPARAM lParam) {
	CEllipseWindow* window =
		(CEllipseWindow*)GetWindowLongPtr(handle, GWL_USERDATA);
	switch (message) {
	case WM_NCCREATE:
		window = (CEllipseWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
		SetWindowLongPtr(handle, GWL_USERDATA, (LONG_PTR)window);
		window->OnNCCreate(handle);
		return DefWindowProc(handle, message, wParam, lParam);
	case WM_CREATE:
		window->OnCreate();
		return DefWindowProc(handle, message, wParam, lParam);
	case WM_PAINT:
		window->OnPaint();
		return 0;
	case WM_DESTROY:
		window->OnDestroy();
		return 0;
	case WM_TIMER:
		window->OnTimer();
		return 0;
	case WM_LBUTTONDOWN:
		SetFocus(handle);
		return 0;
	default:
		return DefWindowProc(handle, message, wParam, lParam);
	}
}