#include "../include/HeaderApp.hpp"
#include "../include/resource.hpp"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR pCmd, int cmdShow) {

    LPCWSTR kNameWnd = L"Test DX11";
    LPCWSTR kNameWndClass = L"DX11_CLASS";
    const unsigned int kWidth = 1920U;
    const unsigned int kHeight = 1080U;
    const bool kWindowed = MessageBoxW(nullptr, L"フルスクリーンで起動しますか", L"確認", MB_YESNO) == IDNO;

    D3DManager dmanager = D3DManager();
    if (!dmanager.init(hInst, cmdShow, kNameWnd, kNameWndClass, kWidth, kHeight, kWindowed))
        return 1;

    InputManager imanager = InputManager();
    imanager.addKeycode(12, VK_RIGHT, GAMEPAD_KEYTYPE::Buttons, XINPUT_GAMEPAD_DPAD_RIGHT);

    AudioManager amanager = AudioManager();
    amanager.init();

    Camera camera = Camera();
    dmanager.createCamera((float)kWidth, (float)kHeight, &camera);
    camera.posZ = -300.0f;
    dmanager.applyCamera(&camera);

    Texture tex = Texture();
    if(!dmanager.createTexture(TEX_TEST, &tex))
        return 1;
    dmanager.applyTexture(&tex);

    ModelSquare model = ModelSquare();
    if(!model.init(&dmanager))
        return 1;

    Audio audio = Audio();
    if (!amanager.createAudio(MUS_TEST, &audio))
        return 1;
    audio.play();

   int cnt = 0;

    MSG msg;
    while (true) {
        if (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else {
            cnt++;
            if (cnt == 200) audio.stop();
            if (cnt == 300) audio.restart();
            imanager.inspect();
            if(imanager.getKey(12) & 0b011)
                camera.posX++;
            dmanager.applyCamera(&camera);
            dmanager.drawBegin();
            dmanager.drawModel(&model);
            dmanager.drawEnd();
        }
    }

    UnregisterClassW(kNameWndClass, hInst);
    return 0;
}

