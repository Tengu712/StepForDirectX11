#include "../include/HeaderDX11.hpp"

HWND* D3DManager::getWindowHandle (){
    return &hWnd;
}

D3DInf* D3DManager::getD3DInformation(){
    return &inf;
}
