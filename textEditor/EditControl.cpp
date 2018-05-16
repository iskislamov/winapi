#include "EditControl.h"

static const int ID_EDITCHILD = 100;

EditControl::EditControl() { handle_ = 0; }

EditControl::~EditControl() {}

bool EditControl::Create(HWND& handleParent) {
	handle_ =
		CreateWindowEx(0, L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE,
			CW_USEDEFAULT, NULL, CW_USEDEFAULT, NULL, handleParent,
			HMENU(ID_EDITCHILD), HINSTANCE(GetModuleHandle(0)), this);
	return handle_ != NULL;
}

HWND EditControl::GetHandle() { return handle_; }

void EditControl::InitFont() {
	HFONT font =
		CreateFont(20, 0, 0, 0, FW_DONTCARE, 0, 0, 0, 0, 0, 0, 0, 0, L"Tahoma");
	SendMessage(handle_, WM_SETFONT, reinterpret_cast<WPARAM>(font), true);
}

void EditControl::DestroyFont() {
	HFONT font = reinterpret_cast<HFONT>(SendMessage(handle_, WM_GETFONT, 0, 0));
	SendMessage(handle_, WM_SETFONT,
		reinterpret_cast<WPARAM>(GetStockObject(SYSTEM_FONT)), true);
	DeleteObject(font);
}

int EditControl::GetFontSize() {
	HFONT oldFont =
		reinterpret_cast<HFONT>(SendMessage(handle_, WM_GETFONT, 0, 0));
	LOGFONT fontAttributes;
	::GetObject(oldFont, sizeof(fontAttributes), &fontAttributes);
	return fontAttributes.lfHeight;
}

void EditControl::SetFontSize(int size) {
	HFONT oldFont =
		reinterpret_cast<HFONT>(SendMessage(handle_, WM_GETFONT, 0, 0));
	LOGFONT fontAttributes;
	::GetObject(oldFont, sizeof(fontAttributes), &fontAttributes);
	fontAttributes.lfHeight = size;
	HFONT font = ::CreateFontIndirect(&fontAttributes);
	SendMessage(handle_, WM_SETFONT, reinterpret_cast<WPARAM>(font), true);
	DeleteObject(oldFont);
}

void EditControl::OnNCCreate(HWND& handle) { handle_ = handle; }

void EditControl::OnCreate() {}

void EditControl::OnDestroy() { PostQuitMessage(0); }

LRESULT EditControl::windowProc(HWND handle, UINT message, WPARAM wParam,
	LPARAM lParam) {
	switch (message) {
	case WM_NCCREATE: {
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		EditControl* windowPtr =
			reinterpret_cast<EditControl*>(pCreate->lpCreateParams);
		SetWindowLongPtr(handle, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(windowPtr));
		windowPtr->OnNCCreate(handle);
		return true;
	}
	case WM_CREATE: {
		EditControl* windowPtr = reinterpret_cast<EditControl*>(
			GetWindowLongPtr(handle, GWLP_USERDATA));
		windowPtr->OnCreate();
		break;
	}
	case WM_DESTROY: {
		EditControl* windowPtr = reinterpret_cast<EditControl*>(
			GetWindowLongPtr(handle, GWLP_USERDATA));
		windowPtr->OnDestroy();
		break;
	}
	default:
		return DefWindowProc(handle, message, wParam, lParam);
	}
	return false;
}