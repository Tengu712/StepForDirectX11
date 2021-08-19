#include "../include/HeaderDX11.hpp"

#include <thread>

void Update(bool* isPlaying, int nABPS, HMMIO hMmio, IXAudio2SourceVoice* pSVoice, VoiceCallback* pCallback) {
    int cntBuf = 0;
    XAUDIO2_BUFFER bufXAudio;
    ZeroMemory(&bufXAudio, sizeof(XAUDIO2_BUFFER));

    BYTE** buf = new BYTE*[2];
    for (int i = 0; i < 2; ++i) {
        buf[i] = new BYTE[nABPS];
    }

    pSVoice->Start();
    *isPlaying = true;

    do {
        int size = mmioRead(hMmio, (HPSTR)buf[cntBuf], nABPS);
        if (size <= 0)
            break;
        bufXAudio.AudioBytes = size;
        bufXAudio.pAudioData = buf[cntBuf];
        if (FAILED(pSVoice->SubmitSourceBuffer(&bufXAudio)))
            break;
        if (++cntBuf >= 2)
            cntBuf = 0;
    } while (*isPlaying && WaitForSingleObject(pCallback->event, INFINITE) == WAIT_OBJECT_0);

    pSVoice->Stop();

    for(int i = 0; i < 2; ++i){
        delete[] buf[i];
    }
    delete[] buf;
}

Audio::Audio() : isPlaying(false), hThread(nullptr), nABPS(0), hMmio(nullptr), pSVoice(nullptr) {}

Audio::~Audio() {
    if (pSVoice != nullptr)
        pSVoice->DestroyVoice();
    if (hMmio != nullptr)
        mmioClose(hMmio, MMIO_FHOPEN);
}

void Audio::play() {
    std::thread th = std::thread(Update, &isPlaying, nABPS, hMmio, pSVoice, &callback);
    th.detach();
}

void Audio::stop() {
    isPlaying = false;
}
