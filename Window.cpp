#include "JApplication.h"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

using namespace Javelin;

//
// CWindow
//

Application::CWindow::CWindow() noexcept :
	m_hInst(HINST_THISCOMPONENT),
	m_hWnd(nullptr){
}

Application::CWindow::~CWindow() noexcept{
}

void Application::CWindow::Cleanup() noexcept {
	UnregisterClass(m_wndClassName.c_str(), m_hInst);
}

int Application::CWindow::MainLoop() noexcept{
	MSG msg;
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT) {
		return 1;
	}
	return 0;
}

void Application::CWindow::Initialize(
	const std::string& appName, const std::string& wndClassName,
	UINT width, UINT height,
	LRESULT(*WndProc)(HWND, UINT, UINT, LONG),
	HICON icon, HCURSOR cursor) {
	m_appName = appName;
	m_wndClassName = wndClassName;

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInst;
	wc.hCursor = cursor;
	wc.hIcon = icon;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = wndClassName.c_str();
	
	WriteLog("ウィンドウクラスの作成");
	if (!RegisterClass(&wc)) {
		WriteLog("ウィンドウクラスの作成に失敗しました");
		throw -1;
	}

	RECT rcWnd;
	rcWnd.top = 0;
	rcWnd.left = 0;
	rcWnd.right = width;
	rcWnd.bottom = height;
	AdjustWindowRect(&rcWnd, WS_OVERLAPPEDWINDOW, false);

	m_hWnd = CreateWindow(appName.c_str(), wndClassName.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top,
		nullptr, nullptr, m_hInst, nullptr);
	WriteLog("ウィンドウの作成");
	if (!m_hWnd) {
		WriteLog("ウィンドウの作成に失敗しました");
		throw -1;
	}

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);
}
