#include "../include/HeaderDX11.hpp"

D3DManager::D3DManager() {
    hWnd = nullptr;
}

void D3DManager::quit(HINSTANCE hInst) {
    UnregisterClassW(L"DX11_CLASS", hInst);
}

HWND* D3DManager::getWindowHandle() {
    return &hWnd;
}

