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

struct D3DInf {
    // General
    ComPtr<ID3D11Device> pDevice;
    ComPtr<ID3D11DeviceContext> pImContext;
    ComPtr<IDXGISwapChain> pSwapChain;
    ComPtr<ID3D11RenderTargetView> pRTView;
    ComPtr<ID3D11DepthStencilView> pDSView;
    D3D11_VIEWPORT viewport;
};

class D3DManager {
private:
    HWND hWnd;
    D3DInf inf;

public:
    D3DManager();

    // General
    bool init(HINSTANCE hInst, int cmdShow, LPCWSTR wndName, LPCWSTR wndClassName, unsigned int width,
        unsigned int height, bool windowed);
    void drawBegin();
    void drawEnd();
    HWND* getWindowHandle();
    D3DInf* getD3DInformation();

    // Debug
    void DebugBox(int i);
    void DebugBox(const char* str);
};

#endif

