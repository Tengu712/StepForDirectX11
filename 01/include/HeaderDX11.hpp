#pragma once
#ifndef _HEADER_HPP_
#define _HEADER_HPP_

#include <DirectXMath.h>
#include <d3d11.h>
#include <windows.h>
#include <wrl/client.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

class D3DManager {
private:
    HWND hWnd;
public:
    D3DManager();
    // General
    bool init(HINSTANCE hInst, int cmdShow, LPCWSTR wndName, LPCWSTR wndClassName, unsigned int width,
        unsigned int height, bool windowed);
    // Debug
    void DebugBox(int i);
    void DebugBox(const char* str);
};

#endif