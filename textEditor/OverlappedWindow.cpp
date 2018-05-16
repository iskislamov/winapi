#include "resource.h"
#include <cmath>

#include "EditControl.h"
#include "OverlappedWindow.h"
#include "Settings.h"

OverlappedWindow::OverlappedWindow() {
	handle_ = 0;
	empty_ = true;
	editControl_ = EditControl();
	setting_ = Setting();
	fontColour_ = RGB(0, 0, 0);
	backgroundColour_ = RGB(255, 255, 255);
}

OverlappedWindow::~OverlappedWindow() {}

bool OverlappedWindow::RegisterClass() {
	WNDCLASSEX window;
	::ZeroMemory(&window, sizeof(window));
	window.cbSize = sizeof(window);
	window.style = CS_HREDRAW | CS_VREDRAW;
	window.lpfnWndProc = windowProc;
	window.hInstance = GetModuleHandle(0);
	window.lpszClassName = L"OverlappedWindow";
	window.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	window.hIcon = static_cast<HICON>(::LoadImage(window.hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
		32, 32, LR_DEFAULTCOLOR));
	window.hIconSm = static_cast<HICON>(::LoadImage(window.hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
		16, 16, LR_DEFAULTCOLOR));
	return (::RegisterClassEx(&window) != 0);
}

bool OverlappedWindow::Create()
{
	handle_ = CreateWindowEx(WS_EX_LAYERED, L"OverlappedWindow", NULL,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, NULL,
		CW_USEDEFAULT, NULL, HWND(NULL), NULL,
		HINSTANCE(GetModuleHandle(0)), this);
	SetLayeredWindowAttributes(handle_, 0, 255, LWA_ALPHA);
	return handle_ != NULL;
}

void OverlappedWindow::Show(int cmdShow)
{
	wchar_t title[10];
	::LoadString(GetModuleHandle(0), IDS_TITLE, title, sizeof(title) / sizeof(wchar_t));
	::SetWindowText(handle_, title);
	HMODULE module = GetModuleHandle(0);
	HRSRC customResource = FindResource(module,	MAKEINTRESOURCE(IDR_CUSTOM1), L"CUSTOM");
	HGLOBAL resourceData = LoadResource(module, customResource);
	DWORD size = ::SizeofResource(module, customResource);
	wchar_t* data = static_cast<wchar_t*>(::LockResource(resourceData));
	wchar_t* text = new wchar_t[size / sizeof(wchar_t) + 1];
	memcpy(text, data, size);
	text[size/ sizeof(wchar_t)] = L'\0';
	editControl_.InitFont();
	::SetWindowText(editControl_.GetHandle(), text);
	ShowWindow(handle_, cmdShow);
}

HWND OverlappedWindow::GetHandle()
{
	return handle_;
}

HWND OverlappedWindow::GetDialogHandle()
{
	return setting_.GetHandle();
}

COLORREF OverlappedWindow::GetFontColour()
{
	return fontColour_;
}

COLORREF OverlappedWindow::GetBackgroundColour()
{
	return backgroundColour_;
}

void OverlappedWindow::SetFontColour(COLORREF colour)
{
	fontColour_ = colour;
}

void OverlappedWindow::SetBackgroundColour(COLORREF colour)
{
	backgroundColour_ = colour;
}

void OverlappedWindow::OnNCCreate(HWND& handle)
{
	handle_ = handle;
}

void OverlappedWindow::OnCreate()
{
	editControl_.Create(handle_);
}

void OverlappedWindow::OnSize()
{
	RECT rect;
	::GetClientRect(handle_, &rect);
	SetWindowPos(editControl_.GetHandle(), 0, rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top, 0);
}

LRESULT OverlappedWindow::OnControlColourEdit(HDC hdc)
{
	SetBkColor(hdc, backgroundColour_);
	SetDCBrushColor(hdc, backgroundColour_);
	SetTextColor(hdc, fontColour_);
	return reinterpret_cast<LRESULT>(GetStockObject(DC_BRUSH));
}

void OverlappedWindow::OnTextChange()
{
	if( ::GetWindowTextLength(editControl_.GetHandle()) == 0 ) {
		empty_ = true;
	} else {
		empty_ = false;
	}
}

void OverlappedWindow::OnClose()
{
	if( !empty_) {
		int messageBoxID = ::MessageBox(handle_,
			L"Do you want to save your text?", L"Warning", MB_YESNOCANCEL);
		switch( messageBoxID ) 
		{
		case IDYES:
			saveFile();
		case IDNO:
			DestroyWindow(handle_);
			break;
		default:
			return;
		}
	} else {
		DestroyWindow(handle_);
	}
}

void OverlappedWindow::OnDestroy()
{
	editControl_.DestroyFont();
	PostQuitMessage(0);
}

void OverlappedWindow::saveFile()
{
	OPENFILENAME fileNameStruct;
	WCHAR fileName[MAX_PATH] = L"";
	ZeroMemory(&fileNameStruct, sizeof(fileNameStruct));
	fileNameStruct.lStructSize = sizeof(fileNameStruct);
	fileNameStruct.hwndOwner = handle_;
	fileNameStruct.lpstrFilter = (LPCWSTR)L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	fileNameStruct.lpstrFile = fileName;
	fileNameStruct.nMaxFile = MAX_PATH;
	fileNameStruct.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
	fileNameStruct.lpstrDefExt = L"txt";
	GetSaveFileName(&fileNameStruct);
	HANDLE fileHandle = CreateFile(fileNameStruct.lpstrFile,
		GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	wchar_t textLength = SendMessage(editControl_.GetHandle(), WM_GETTEXTLENGTH, 0, 0);
	wchar_t* text = new wchar_t[textLength + 1];
	SendMessage(editControl_.GetHandle(), WM_GETTEXT, textLength + 1, (LPARAM)text);
	DWORD bytesWritten;
	static const BYTE BOM[] = { 0xFF, 0xFE };
	WriteFile(fileHandle, BOM, sizeof(BOM), &bytesWritten, NULL);
	WriteFile(fileHandle, text, textLength*sizeof(wchar_t), &bytesWritten, NULL);
	delete[] text;
	CloseHandle(fileHandle);
}

void OverlappedWindow::showSettings()
{
	setting_.SetEditControl(editControl_);
	setting_.Create(handle_);
}

LRESULT OverlappedWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message ) {
		case WM_NCCREATE:
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(pCreate->lpCreateParams);
			SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPtr));
			windowPtr->OnNCCreate(handle);
			return true;
		}
		case WM_CREATE:
		{
			OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
			windowPtr->OnCreate();
			break;
		}
		case WM_SIZE:
		{
			OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
			windowPtr->OnSize();
			break;
		}
		case WM_CTLCOLOREDIT:
		{
			OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
			HDC hdc = reinterpret_cast<HDC>(wParam);
			return windowPtr->OnControlColourEdit(hdc);
		}
		case WM_COMMAND:
		{
			if( HIWORD(wParam) == EN_CHANGE ) {
				OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
				windowPtr->OnTextChange();
				break;
			}
			switch( LOWORD(wParam) ) {
				case ID_FILE_SAVE:
				{
					OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
					windowPtr->saveFile();
					break;
				}
				case ID_FILE_EXIT:
				{
					OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
					windowPtr->OnClose();
					break;
				}
				case ID_VIEW_SETTINGS:
				{
					OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
					windowPtr->showSettings();
					break;
				}
				case ID_KILL:
				{
					OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
					windowPtr->OnDestroy();
					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		{
			OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
			windowPtr->OnClose();
			break;
		}
		case WM_DESTROY:
		{
			OverlappedWindow* windowPtr = reinterpret_cast<OverlappedWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
			windowPtr->OnDestroy();
			break;
		}
		default:
			return DefWindowProc(handle, message, wParam, lParam);
	}
	return false;
}
