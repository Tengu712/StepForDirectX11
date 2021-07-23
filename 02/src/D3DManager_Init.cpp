#include "../include/HeaderDX11.hpp"

LRESULT WINAPI WndProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool D3DManager::init(HINSTANCE hInst, int cmdShow, LPCWSTR nameWnd, unsigned int width,
    unsigned int height, bool windowed) {
    try {

        {
            const DWORD kDwStyle = windowed ? WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX : WS_POPUP;
            const DWORD kDwExStyle = windowed ? 0 : WS_EX_TOPMOST;
            const int kCmdShow = windowed ? SW_SHOW : SW_SHOWMAXIMIZED;

            WNDCLASSEXW wcex = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, hInst, nullptr, nullptr, nullptr,
                nullptr, L"DX11_CLASS", nullptr};
            if (!RegisterClassExW(&wcex))
                throw "Error: Failed to register window class.";

            RECT rect;
            rect = {0, 0, (long)width, (long)height};
            AdjustWindowRectEx(&rect, kDwStyle, false, kDwExStyle);

            hWnd = CreateWindowExW(kDwExStyle, L"DX11_CLASS", nameWnd, kDwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
                rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInst, nullptr);
            if (!hWnd)
                throw "Error: Failed to create window handle.";

            ShowWindow(hWnd, cmdShow);
            UpdateWindow(hWnd);
            ShowCursor(windowed);
        }

    } catch (const char* error) {
        DebugBox(error);
        return false;
    }

    return true;
}

