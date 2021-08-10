#include "../include/HeaderApp.hpp"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR pCmd, int cmdShow) {

    LPCWSTR kNameWnd = L"Test DX11";
    LPCWSTR kNameWndClass = L"DX11_CLASS";
    const unsigned int kWidth = 1920U;
    const unsigned int kHeight = 1080U;
    const bool kWindowed = MessageBoxW(nullptr, L"フルスクリーンで起動しますか", L"確認", MB_YESNO) == IDNO;

    D3DManager dmanager = D3DManager();
    if (!dmanager.init(hInst, cmdShow, kNameWnd, kNameWndClass, kWidth, kHeight, kWindowed))
        return 1;

    Camera camera = Camera();
    dmanager.createCamera((float)kWidth, (float)kHeight, &camera);
    camera.posZ = -500.0f;
    dmanager.applyCamera(&camera);

    Texture tex = Texture();
    if(!dmanager.createTexture(0, &tex))
        return 1;
    dmanager.applyTexture(&tex);

    ModelSquare model = ModelSquare();
    if(!model.init(&dmanager))
        return 1;

    MSG msg;
    while (true) {
        if (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else {
            dmanager.drawBegin();
            dmanager.drawModel(&model);
            dmanager.drawEnd();
        }
    }

    UnregisterClassW(kNameWndClass, hInst);
    return 0;
}

