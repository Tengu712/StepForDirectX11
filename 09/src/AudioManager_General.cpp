#include "../include/HeaderDX11.hpp"

AudioManager::AudioManager() : pXAudio(nullptr), pMVoice(nullptr) {}

AudioManager::~AudioManager() {
    if (pMVoice != nullptr)
        pMVoice->DestroyVoice();
    CoUninitialize();
}

bool AudioManager::init() {
    try {
        if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
            throw "Failed to CoInitializeEx.";
        if (FAILED(XAudio2Create(pXAudio.GetAddressOf())))
            throw "Failed to create XAudio2.";
        if (FAILED(pXAudio->CreateMasteringVoice(&pMVoice)))
            throw "Failed to create MasteringVoice.";
    } catch (const char* e) {
        MessageBoxA(nullptr, e, "Warning", MB_OK);
        return false;
    }
    return true;
}

bool AudioManager::createAudio(unsigned int id, Audio* pAudio) {
    MMIOINFO info;
    ZeroMemory(&info, sizeof(MMIOINFO));
    pAudio->hMmio = mmioOpen("test.wav", &info, MMIO_READ);
    if (!pAudio->hMmio) {
        MessageBoxA(nullptr, "Failed to open wave file.", "Error", MB_OK);
        return false;
    }

    try {
        MMCKINFO chunkRiff;
        chunkRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
        if (MMSYSERR_NOERROR != mmioDescend(pAudio->hMmio, &chunkRiff, NULL, MMIO_FINDRIFF))
            throw "Failed to descend to riff chunk.";

        MMCKINFO chunk;
        chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
        if (MMSYSERR_NOERROR != mmioDescend(pAudio->hMmio, &chunk, &chunkRiff, MMIO_FINDCHUNK))
            throw "Failed to descend to format chunk.";

        WAVEFORMATEX wfex;
        ZeroMemory(&wfex, sizeof(WAVEFORMATEX));
        if (chunk.cksize != mmioRead(pAudio->hMmio, (HPSTR)&wfex, chunk.cksize))
            throw "Failed to read format.";

        if (FAILED(pXAudio->CreateSourceVoice(
                &pAudio->pSVoice, &wfex, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &pAudio->callback)))
            throw "Failed to create soucevoice.";

        chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
        if (MMSYSERR_NOERROR != mmioDescend(pAudio->hMmio, &chunk, &chunkRiff, MMIO_FINDCHUNK))
            throw "Failed to descend to data chunk.";

        pAudio->nABPS = wfex.nAvgBytesPerSec;

    } catch (const char* error) {
        MessageBoxA(nullptr, error, "Error", MB_OK);
        mmioClose(pAudio->hMmio, MMIO_FHOPEN);
        return false;
    }

    return true;
}