#include "WindowsProject.h"
#include "framework.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include <iostream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;					 // current instance
WCHAR szTitle[MAX_LOADSTRING];		 // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINDOWSPROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable =
		LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance,
		nullptr);

	if (!hWnd) {
		return FALSE;
	}

	// 透明度
	SetLayeredWindowAttributes(hWnd, 0, 128, LWA_ALPHA);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void createSkiaContext(HDC &hdc, sk_sp<SkSurface> &skSurface, HDC &hdcMemory,
	HBITMAP &hBitmap, LONG width, LONG height) {
	if (hdcMemory) {
		DeleteDC(hdcMemory);
		hdcMemory = nullptr;
	}
	if (hBitmap) {
		DeleteObject(hBitmap);
		hBitmap = nullptr;
	}
	hdcMemory = CreateCompatibleDC(hdc);

	BITMAPINFO info = {};
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = width;
	info.bmiHeader.biHeight = -height;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 32;
	info.bmiHeader.biCompression = BI_RGB;

	void *pixels = nullptr;
	hBitmap = CreateDIBSection(hdc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
	if (!hBitmap) {
		std::cerr << "CreateDIBSection 出错" << std::endl;
	}

	SelectObject(hdcMemory, hBitmap);

	SkImageInfo imageInfo = SkImageInfo::MakeN32Premul(width, height);
	skSurface =
		SkSurfaces::WrapPixels(imageInfo, pixels, ((width * 32 + 31) / 32) * 4);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static sk_sp<SkSurface> skSurface;
	static HDC hdcMemory;
	static HBITMAP hBitmap;

	switch (message) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	} break;
	case WM_CREATE: {
		HDC hdc = GetDC(hWnd);
		RECT rect;
		GetClientRect(hWnd, &rect);

		createSkiaContext(
			hdc, skSurface, hdcMemory, hBitmap, rect.right, rect.bottom);

		std::cout << "create x: " << rect.left << " y: " << rect.top
				  << " w: " << rect.right << " h: " << rect.bottom << std::endl;

		ReleaseDC(hWnd, hdc);
	} break;
	case WM_SIZE: {
		HDC hdc = GetDC(hWnd);
		RECT rect;
		GetClientRect(hWnd, &rect);

		createSkiaContext(
			hdc, skSurface, hdcMemory, hBitmap, rect.right, rect.bottom);

		std::cout << "resize x: " << rect.left << " y: " << rect.top
				  << " w: " << rect.right << " h: " << rect.bottom << std::endl;

		ReleaseDC(hWnd, hdc);
		InvalidateRect(hWnd, nullptr, TRUE);
	} break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		if (skSurface) {
			SkCanvas *canvas = skSurface->getCanvas();
			if (canvas) {
				canvas->clear(SK_ColorWHITE);

				SkPaint paint;
				paint.setAntiAlias(true);
				paint.setColor(SK_ColorBLUE);
				canvas->drawRect(SkRect::MakeXYWH(10, 10, 60, 40), paint);
			}
		}
		if (!BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, hdcMemory,
				0, 0, SRCCOPY)) {
			MessageBoxW(hWnd, L"BitBlt 执行出错", L"BitBlt 执行出错", MB_OK);
		}

		EndPaint(hWnd, &ps);
	} break;
	case WM_DESTROY:
		if (hBitmap) {
			DeleteObject(hBitmap);
			hBitmap = nullptr;
		}
		if (hdcMemory) {
			DeleteDC(hdcMemory);
			hdcMemory = nullptr;
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}