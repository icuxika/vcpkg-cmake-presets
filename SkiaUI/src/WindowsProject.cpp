#include "WindowsProject.h"
#include "framework.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathEffect.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkCornerPathEffect.h"
#include <iostream>
#include <string>
#include <vector>

#define MAX_LOADSTRING 100

// 全局变量
HINSTANCE hInst;			   // 存储当前应用程序实例的句柄
WCHAR szTitle[MAX_LOADSTRING]; // 应用程序标题
WCHAR szWindowClass
	[MAX_LOADSTRING]; // 用于定义主窗口的类名，在窗口注册等操作中会使用到这个名称来标识窗口所属的类
HHOOK hKeyboardHook;  // 全局键盘钩子句柄

// 函数声明，用于注册窗口类，在后续的程序初始化阶段会调用这个函数来完成窗口类的注册操作
ATOM myRegisterClass(HINSTANCE hInstance);
// 函数声明，用于初始化应用程序实例，比如创建窗口、显示窗口等操作都在此函数中进行
BOOL initInstance(HINSTANCE, int);
// 窗口过程函数声明，用于处理窗口接收到的各种消息，如鼠标消息、键盘消息、菜单消息等，是窗口消息处理的核心函数
LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
// 关于对话框的回调函数声明，用于处理关于对话框的相关消息，比如对话框的初始化、按钮点击等操作
INT_PTR CALLBACK about(HWND, UINT, WPARAM, LPARAM);
// 键盘钩子回调函数声明，用于拦截和处理键盘消息，在这里可以检测全局快捷键以及打印按下或释放的按键信息等
LRESULT CALLBACK keyboardProc(int code, WPARAM wParam, LPARAM lParam);
// 异常捕获
LONG WINAPI exceptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	SetUnhandledExceptionFilter(exceptionFilter);

	// 将进程默认 DPI 感知设置为系统 DPI 感知
	SetProcessDPIAware();

	// 注册键盘钩子
	hKeyboardHook =
		SetWindowsHookExW(WH_KEYBOARD_LL, keyboardProc, hInstance, 0);
	if (hKeyboardHook == NULL) {
		MessageBoxW(NULL, L"全局键盘钩子注册失败", L"错误", MB_OK);
	}

	// 从 WindowsProject.rc
	// 文件中的字符串资源表（STRINGTABLE）读取应用程序标题和窗口类名对应的字符串资源，
	// 分别存储到 szTitle 和 szWindowClass 变量中，MAX_LOADSTRING
	// 用于指定读取字符串的最大长度
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINDOWSPROJECT, szWindowClass, MAX_LOADSTRING);
	myRegisterClass(hInstance);

	// 使 std::wcout 能够输出中文且不乱码
	std::locale::global(std::locale(".UTF8"));
	std::wcout << L"szTitle: " << szTitle << std::endl;
	std::wcout << L"szWindowClass: " << szWindowClass << std::endl;

	// 调用 InitInstance 函数进行应用程序的初始化操作，如果初始化失败（返回
	// FALSE），则程序直接退出并返回相应的错误码
	if (!initInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	// 加载应用程序的快捷键表，通过 MAKEINTRESOURCE
	// 宏将快捷键表资源标识符转换为相应的资源句柄，
	// 后续在消息循环中会根据这个快捷键表来处理快捷键相关的消息
	HACCEL hAccelTable =
		LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT));

	MSG msg;

	// 消息循环是 Windows
	// 应用程序的核心部分，不断从消息队列中获取消息并进行处理，直到接收到
	// WM_QUIT 消息退出循环
	while (GetMessage(&msg, nullptr, 0, 0)) {
		// 判断当前消息是否是快捷键消息，如果是快捷键消息且被
		// TranslateAccelerator 函数处理了（返回非零值）， 则不会再进入下面的
		// TranslateMessage 和 DispatchMessage 流程，避免重复处理
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			// 将某些键盘按键消息（如按键按下）转换为字符消息（如对应的 ASCII
			// 字符消息），方便后续在窗口过程函数中进行字符输入相关处理
			TranslateMessage(&msg);
			// 将消息分发给对应的窗口过程函数（WndProc）进行处理，根据消息中的窗口句柄找到相应的窗口过程函数来响应消息内容
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

// 函数定义，用于注册窗口类
// 通过填充 WNDCLASSEXW
// 结构体的各个字段来定义窗口类的属性，如窗口风格、窗口过程函数指针、图标、光标、背景颜色、菜单名等信息，
// 最后调用 RegisterClassExW
// 函数将定义好的窗口类注册到系统中，返回注册结果（ATOM
// 类型的原子值，用于标识窗口类）
ATOM myRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
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

// 函数定义，用于初始化应用程序实例，主要是创建和显示主窗口
// 首先将传入的实例句柄存储到全局变量 hInst 中，方便后续其他地方使用，
// 然后通过 CreateWindowW
// 函数创建主窗口，根据指定的窗口类名、标题、窗口风格等参数来创建窗口，
// 如果窗口创建成功，调用 ShowWindow 和 UpdateWindow
// 函数分别显示窗口并触发窗口的首次重绘操作，最后返回初始化结果（成功返回
// TRUE，失败返回 FALSE）
BOOL initInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance,
		nullptr);

	if (!hWnd) {
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

// 函数定义，用于创建 Skia
// 绘图上下文相关的资源，包括内存设备上下文（hdcMemory）、位图（hBitmap）以及
// SkSurface 对象等
// 首先释放之前可能存在的内存设备上下文和位图资源（如果有的话），然后创建与传入的设备上下文（hdc）兼容的内存设备上下文，
// 接着定义位图信息结构体（BITMAPINFO）来指定要创建的位图的属性，如宽度、高度、位深度、颜色格式等，
// 通过 CreateDIBSection
// 函数创建一个与设备无关的位图（DIB），并将其选入内存设备上下文， 最后使用
// SkSurfaces::WrapPixels 函数基于创建好的位图像素数据等信息创建 SkSurface
// 对象，用于后续的 Skia 绘图操作
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

class SkiaUIElementNode {
  public:
	int ElementType; // 标识元素类型，比如按钮、文本框、容器等
	SkRect LayoutBounds; // 布局区域，确定自身在父容器中的位置和尺寸
	SkPaint Paint; // 绘制相关属性
	std::vector<std::shared_ptr<SkiaUIElementNode>>
		Children; // 子节点指针列表，体现层次关系
	virtual void draw(SkCanvas *canvas) {
		// 绘制自身的逻辑，根据elementType调用相应Skia绘制函数，类似前面图形对象列表中的绘制逻辑
		if (ElementType ==
			1) { // 假设按钮类型为1，绘制按钮（示例简单以矩形表示）
			canvas->drawRect(LayoutBounds, Paint);
		}
		// 绘制子节点
		for (const auto &child : Children) {
			child->draw(canvas);
		}
	}
};

// 窗口过程函数定义，用于处理主窗口接收到的各种消息
LRESULT CALLBACK wndProc(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static sk_sp<SkSurface> skSurface;
	static HDC hdcMemory;
	static HBITMAP hBitmap;

	switch (message) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 根据菜单命令的标识符（wmId）来处理不同的菜单选择操作
		switch (wmId) {
		case IDM_SET_TRANSPARENCY: {
			// 设置窗口为半透明
			SetWindowLong(hWnd, GWL_EXSTYLE,
				GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hWnd, 0, (255 * 70) / 100, LWA_ALPHA);
		} break;
		case IDM_CLOSE_TRANSPARENCY: {
			// 设置窗口为不透明
			SetWindowLong(hWnd, GWL_EXSTYLE,
				GetWindowLong(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
			RedrawWindow(hWnd, NULL, NULL,
				RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
		} break;
		case IDM_SHOW_WINDOW: {
			if (!RegisterHotKey(hWnd, 1, MOD_CONTROL | MOD_ALT, 0x5A)) {
				MessageBoxW(hWnd, L"注册快捷键失败", L"错误", MB_OK);
			}
		} break;
		case IDM_HIDE_WINDOW: {
			UnregisterHotKey(hWnd, 1);
		} break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, about);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			// 对于其他未处理的菜单命令，交给默认的窗口过程函数（DefWindowProc）来处理，确保系统默认的行为得以执行
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	} break;
	case WM_HOTKEY: {
		if (wParam == 1) {
			WINDOWPLACEMENT wp = {sizeof(WINDOWPLACEMENT)};
			GetWindowPlacement(hWnd, &wp);
			// 判断窗口当前是否处于最小化状态
			if (wp.showCmd == SW_SHOWMINIMIZED) {
				// 如果窗口处于最小化状态，则调用 ShowWindow
				// 函数将窗口恢复到之前的显示状态
				ShowWindow(hWnd, SW_RESTORE);
				// 如果窗口不是最小化状态，则调用 ShowWindow
				// 函数将窗口最小化
			} else {
				ShowWindow(hWnd, SW_MINIMIZE);
			}
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

				SkRect buttonRect = SkRect::MakeXYWH(100, 60, 360, 120);
				SkPaint buttonPaint;
				buttonPaint.setAntiAlias(true);
				buttonPaint.setStyle(SkPaint::kStrokeAndFill_Style);
				buttonPaint.setStrokeWidth(2.0);
				buttonPaint.setColor(SK_ColorBLUE);
				// setPathEffect 会导致程序崩溃，原因未知
				//				buttonPaint.setPathEffect(SkCornerPathEffect::Make(10.0f));
				canvas->drawRoundRect(buttonRect, 10.0f, 10.0f, buttonPaint);
			}
		}
		// 将内存设备上下文（hdcMemory）中的位图内容复制到窗口的设备上下文（hdc）中，实现绘图内容在窗口上的显示，
		// 如果 BitBlt 操作执行出错，会弹出一个消息框提示错误信息
		if (!BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, hdcMemory,
				0, 0, SRCCOPY)) {
			MessageBoxW(hWnd, L"BitBlt 执行出错", L"BitBlt 执行出错", MB_OK);
		}

		EndPaint(hWnd, &ps);
	} break;
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN: {
		// 复杂结构考虑四叉树优化
		if (skSurface) {
			SkCanvas *canvas = skSurface->getCanvas();
			if (canvas) {
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				SkRect rect = SkRect::MakeXYWH(10, 10, 60, 40);
				if (rect.contains(SkIntToScalar(x), SkIntToScalar(y))) {
					if (message == WM_LBUTTONDOWN) {
						std::cout << "鼠标按下在矩形内" << std::endl;
					} else if (message == WM_LBUTTONUP) {
						MessageBoxW(hWnd, L"点击了矩形区域", L"提示", MB_OK);
					}
				}
			}
		}
	} break;
	case WM_CLOSE: {
		if (MessageBoxW(hWnd, L"确定要关闭应用程序吗", L"Skia示例",
				MB_YESNOCANCEL) == IDYES) {
			DestroyWindow(hWnd);
		}
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
		if (hKeyboardHook != NULL) {
			UnhookWindowsHookEx(hKeyboardHook);
			hKeyboardHook = NULL;
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 根据扫描码获取对应的键名，返回一个 std::wstring 类型的宽字符串表示键名
std::wstring keyNameFromScanCode(UINT scanCode) {
	wchar_t buf[32];
	GetKeyNameTextW(scanCode << 16, buf, sizeof(buf));
	return {buf};
}

// 根据虚拟键码获取对应的键名，通过先将虚拟键码转换为扫描码，再调用
// keyNameFromScanCode 函数来实现
std::wstring keyNameFromVirtualKeyCode(DWORD virtualKeyCode) {
	return keyNameFromScanCode(MapVirtualKeyW(virtualKeyCode, MAPVK_VK_TO_VSC));
}

LRESULT CALLBACK keyboardProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code == HC_ACTION) {
		KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
		if (wParam == WM_KEYDOWN) {
			std::cout << "down: " << p->vkCode << std::endl;
			std::wcout << L"down: " << keyNameFromVirtualKeyCode(p->vkCode)
					   << std::endl;
		} else if (wParam == WM_KEYUP) {
			std::cout << "up: " << p->vkCode << std::endl;
			std::wcout << L"up: " << keyNameFromVirtualKeyCode(p->vkCode)
					   << std::endl;
		}
		if (wParam == WM_KEYDOWN) {
			// CTRL + ALT + J 最小化
			if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
				(GetAsyncKeyState(VK_MENU) & 0x8000) && p->vkCode == 'J') {
				// 通过窗口类名（szWindowClass）和窗口标题（szTitle）查找对应的窗口句柄，
				// 前提是窗口类名和标题在系统中是唯一可标识该窗口的，若找到则返回对应的窗口句柄，否则返回
				// NULL
				HWND hwnd = FindWindowW(szWindowClass, szTitle);
				if (hwnd != NULL) {
					// 通过 GetWindowPlacement
					// 函数获取指定窗口（hwnd）的当前显示状态等信息，存储到 wp
					// 结构体中
					WINDOWPLACEMENT wp = {sizeof(WINDOWPLACEMENT)};
					GetWindowPlacement(hwnd, &wp);
					// 判断窗口当前是否处于最小化状态
					if (wp.showCmd == SW_SHOWMINIMIZED) {
						// 如果窗口处于最小化状态，则调用 ShowWindow
						// 函数将窗口恢复到之前的显示状态
						ShowWindow(hwnd, SW_RESTORE);
						// 如果窗口不是最小化状态，则调用 ShowWindow
						// 函数将窗口最小化
					} else {
						ShowWindow(hwnd, SW_MINIMIZE);
					}
				}
			}
		}
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}

INT_PTR CALLBACK about(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		// 使对话框半透明
		SetWindowLong(hDlg, GWL_EXSTYLE,
			GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(hDlg, 0, (255 * 70) / 100, LWA_ALPHA);
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

BOOL dumpFile(
	const std::wstring &strPath, struct _EXCEPTION_POINTERS *exceptionInfo) {
	HANDLE hFile = CreateFileW(strPath.c_str(), GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cerr << "dump文件创建失败" << std::endl;
		return FALSE;
	}
	MINIDUMP_EXCEPTION_INFORMATION m1;
	m1.ThreadId = GetCurrentThreadId();
	m1.ExceptionPointers = exceptionInfo;
	m1.ClientPointers = TRUE;
	MINIDUMP_USER_STREAM_INFORMATION m2;
	m2.UserStreamCount = 0;
	m2.UserStreamArray = NULL;
	MINIDUMP_CALLBACK_INFORMATION m3;
	m3.CallbackRoutine = NULL;
	m3.CallbackParam = NULL;

	BOOL dumpResult = MiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(), hFile, MiniDumpNormal, &m1, &m2, &m3);
	CloseHandle(hFile);
	return dumpResult;
}

LONG WINAPI exceptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo) {
	auto exceptionCode = exceptionInfo->ExceptionRecord->ExceptionCode;
	auto *exceptionAddress = exceptionInfo->ExceptionRecord->ExceptionAddress;

	std::wstring msg = L"未处理的异常，错误代码是 " +
		std::to_wstring(exceptionCode) + L"，异常地址是 " +
		std::to_wstring(reinterpret_cast<uintptr_t>(exceptionAddress));
	MessageBoxW(NULL, msg.c_str(), L"程序异常", MB_OK | MB_ICONERROR);

	wchar_t path[MAX_PATH] = {0};
	GetModuleFileNameW(NULL, path, MAX_PATH);
	PathCchRemoveFileSpec(path, lstrlenW(path));
	std::wstring strPath = path;
	strPath += L"\\dump.dmp";

	if (dumpFile(strPath, exceptionInfo)) {
		return EXCEPTION_EXECUTE_HANDLER;
	}
	// 可以选择返回值来决定系统对异常的后续处理方式
	// EXCEPTION_EXECUTE_HANDLER表示已经处理了异常，系统可以终止进程
	// EXCEPTION_CONTINUE_SEARCH表示让系统继续寻找其他异常处理机制
	return EXCEPTION_CONTINUE_SEARCH;
}