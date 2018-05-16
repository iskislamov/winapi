#include "Settings.h"
#include "resource.h"
#include "CommCtrl.h"
#include "OverlappedWindow.h"
#include "EditControl.h"

Setting::Setting() {
	handle_ = 0;
	editControl_ = EditControl();
	preview_ = false;
	oldFontSize_ = 0;
	fontSize_ = 0;
	oldTransparency_ = 0;
	transparency_ = 0;
	oldFontColour_ = RGB(0, 0, 0);
	fontColour_ = RGB(0, 0, 0);
	oldBackgroundColour_ = RGB(255, 255, 255);
	backgroundColour_ = RGB(255, 255, 255);
	::ZeroMemory(colours_, 16 * sizeof(COLORREF));
}

Setting::~Setting() {}

HWND Setting::GetHandle() { return handle_; }

bool Setting::Create(HWND& windowHandle) {
	OverlappedWindow* window = reinterpret_cast<OverlappedWindow*>(
		GetWindowLongPtr(windowHandle, GWLP_USERDATA));
	CreateDialogParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1),
		windowHandle, dialogProc, reinterpret_cast<LPARAM>(this));
	ShowWindow(handle_, SW_SHOW);
	return (handle_ != 0);
}

void Setting::SetEditControl(EditControl& editControl) {
	editControl_ = editControl;
}

void Setting::OnCheckPreview() {
	HWND parentHandle = GetParent(handle_);
	OverlappedWindow* window = reinterpret_cast<OverlappedWindow*>(
		GetWindowLongPtr(parentHandle, GWLP_USERDATA));
	preview_ = IsDlgButtonChecked(handle_, IDC_CHECK1) == BST_CHECKED;
	if (preview_) {
		window->SetFontColour(fontColour_);
		window->SetBackgroundColour(backgroundColour_);
		editControl_.SetFontSize(fontSize_);
		SetLayeredWindowAttributes(parentHandle, 0, transparency_, LWA_ALPHA);
	}
	else {
		window->SetFontColour(oldFontColour_);
		window->SetBackgroundColour(oldBackgroundColour_);
		editControl_.SetFontSize(oldFontSize_);
		SetLayeredWindowAttributes(parentHandle, 0, oldTransparency_, LWA_ALPHA);
	}
	InvalidateRect(parentHandle, 0, true);
}

void Setting::OnInitDialog(HWND& handle) {
	preview_ = false;
	handle_  = handle;
	HWND parentHandle = GetParent(handle_);
	OverlappedWindow* window = reinterpret_cast<OverlappedWindow*>(
		GetWindowLongPtr(parentHandle, GWLP_USERDATA));
	oldFontSize_ = editControl_.GetFontSize();
	oldBackgroundColour_ = window->GetBackgroundColour();
	oldFontColour_ = window->GetFontColour();
	BYTE buff;
	GetLayeredWindowAttributes(GetParent(handle), 0, &buff, 0);
	oldTransparency_ = buff;
	SendDlgItemMessage(handle, IDC_SLIDER1, TBM_SETRANGE, 1, MAKELONG(8, 72));
	SendDlgItemMessage(handle, IDC_SLIDER1, TBM_SETPOS, 1, oldFontSize_);
	SendDlgItemMessage(handle, IDC_SLIDER2, TBM_SETRANGE, 1, MAKELONG(0, 255));
	SendDlgItemMessage(handle, IDC_SLIDER2, TBM_SETPOS, 1, oldTransparency_);
	fontSize_ = oldFontSize_;
	transparency_ = oldTransparency_;
	backgroundColour_ = oldBackgroundColour_;
	fontColour_ = oldFontColour_;
}

void Setting::OnFontChange() {
	fontSize_ = SendDlgItemMessage(handle_, IDC_SLIDER1, TBM_GETPOS, 0, 0);
	if (preview_) {
		editControl_.SetFontSize(fontSize_);
	}
}

void Setting::OnTransparencyChange() {
	transparency_ = SendDlgItemMessage(handle_, IDC_SLIDER2, TBM_GETPOS, 0, 0);
	if (preview_) {
		SetLayeredWindowAttributes(GetParent(handle_), 0, transparency_, LWA_ALPHA);
	}
}

void Setting::OnFontColourChange() {
	CHOOSECOLOR chooseColour;
	::ZeroMemory(&chooseColour, sizeof(CHOOSECOLOR));
	chooseColour.lStructSize = sizeof(CHOOSECOLOR);
	chooseColour.Flags = CC_RGBINIT | CC_FULLOPEN;
	chooseColour.hwndOwner = handle_;
	chooseColour.lpCustColors = colours_;
	chooseColour.rgbResult = fontColour_;
	if (::ChooseColor(&chooseColour)) {
		HWND parentHandle = GetParent(handle_);
		OverlappedWindow* window = reinterpret_cast<OverlappedWindow*>(
			GetWindowLongPtr(parentHandle, GWLP_USERDATA));
		fontColour_ = chooseColour.rgbResult;
		if (preview_) {
			window->SetFontColour(fontColour_);
			editControl_.SetFontSize(fontSize_);
		}
		else {
			window->SetFontColour(oldFontColour_);
			editControl_.SetFontSize(oldFontSize_);
		}
	}
}

void Setting::OnBackgroundColourChange() {
	CHOOSECOLOR chooseColour;
	::ZeroMemory(&chooseColour, sizeof(CHOOSECOLOR));
	chooseColour.lStructSize = sizeof(CHOOSECOLOR);
	chooseColour.Flags = CC_RGBINIT | CC_FULLOPEN;
	chooseColour.hwndOwner = handle_;
	chooseColour.lpCustColors = colours_;
	chooseColour.rgbResult = backgroundColour_;
	if (::ChooseColor(&chooseColour)) {
		HWND parentHandle = GetParent(handle_);
		OverlappedWindow* window = reinterpret_cast<OverlappedWindow*>(
			GetWindowLongPtr(parentHandle, GWLP_USERDATA));
		backgroundColour_ = chooseColour.rgbResult;
		if (preview_) {
			window->SetBackgroundColour(backgroundColour_);
			editControl_.SetFontSize(fontSize_);
		}
		else {
			window->SetBackgroundColour(oldBackgroundColour_);
			editControl_.SetFontSize(oldFontSize_);
		}
	}
}

void Setting::OnOk() {
	HWND parentHandle = GetParent(handle_);
	OverlappedWindow* window = reinterpret_cast<OverlappedWindow*>(
		GetWindowLongPtr(parentHandle, GWLP_USERDATA));
	window->SetFontColour(fontColour_);
	window->SetBackgroundColour(backgroundColour_);
	editControl_.SetFontSize(fontSize_);
	SetLayeredWindowAttributes(parentHandle, 0, transparency_, LWA_ALPHA);
}

void Setting::OnCancel() {
	HWND parentHandle = GetParent(handle_);
	OverlappedWindow* window = reinterpret_cast<OverlappedWindow*>(
		GetWindowLongPtr(parentHandle, GWLP_USERDATA));
	window->SetFontColour(oldFontColour_);
	window->SetBackgroundColour(oldBackgroundColour_);
	editControl_.SetFontSize(oldFontSize_);
	SetLayeredWindowAttributes(parentHandle, 0, oldTransparency_, LWA_ALPHA);
}

INT_PTR Setting::dialogProc(HWND handle, UINT message, WPARAM wParam,
	LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG: {
		Setting* dialogPtr = reinterpret_cast<Setting*>(lParam);
		SetWindowLongPtr(handle, DWLP_USER,
			reinterpret_cast<LONG_PTR>(dialogPtr));
		dialogPtr->OnInitDialog(handle);
		break;
	}
	case WM_HSCROLL: {
		Setting* dialogPtr =
			reinterpret_cast<Setting*>(GetWindowLongPtr(handle, DWLP_USER));
		int sliderId = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
		if (sliderId == IDC_SLIDER1) {
			dialogPtr->OnFontChange();
		}
		else {
			dialogPtr->OnTransparencyChange();
		}
		break;
	}
	case WM_COMMAND: {
		Setting* dialogPtr =
			reinterpret_cast<Setting*>(GetWindowLongPtr(handle, DWLP_USER));
		switch (LOWORD(wParam)) {
		case IDOK: {
			dialogPtr->OnOk();
			EndDialog(handle, wParam);
			return true;
		}
		case IDCANCEL: {
			dialogPtr->OnCancel();
			EndDialog(handle, wParam);
			return true;
		}
		case IDC_CHECK1: {
			dialogPtr->OnCheckPreview();
			break;
		}
		case IDC_BUTTON1: {
			dialogPtr->OnBackgroundColourChange();
			break;
		}
		case IDC_BUTTON2: {
			dialogPtr->OnFontColourChange();
			break;
		}
		}
	}
	}
	return false;
}