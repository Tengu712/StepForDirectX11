#include "../include/HeaderDX11.hpp"
#include "../include/resource.hpp"

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

bool FindChunk(BYTE* pFile, DWORD sizeRes, DWORD fourcc, DWORD* sizeChunk, DWORD* posChunkData) {
    DWORD offset = 0;
    while (offset < sizeRes) {
        DWORD typeChunk;
        memcpy(&typeChunk, pFile + offset, sizeof(DWORD));
        offset += sizeof(DWORD);

        DWORD sizeChunkTmp;
        memcpy(&sizeChunkTmp, pFile + offset, sizeof(DWORD));
        offset += sizeof(DWORD);

        if (typeChunk == fourcc) {
            *sizeChunk = sizeChunkTmp;
            *posChunkData = offset;
            return true;
        }

        if (typeChunk == fourccRIFF)
            offset += sizeof(DWORD);
        else
            offset += sizeChunkTmp;
    }

    return false;
}

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
    try {
        HRSRC hFileSrc = FindResource(nullptr, MAKEINTRESOURCE(id), "WAVE");
        if (!hFileSrc)
            throw "Failed to find wave resource.";

        HGLOBAL hFileGlb = LoadResource(nullptr, hFileSrc);
        if (!hFileGlb)
            throw "Failed to load wave resource.";

        BYTE* pLock = (BYTE*)LockResource(hFileGlb);
        if (!pLock)
            throw "Failed to lock wave resource.";

        DWORD sizeRes = SizeofResource(nullptr, hFileSrc);
        if (sizeRes == 0)
            throw "Failed to get size of wave resource.";

        DWORD sizeChunk;
        DWORD posChunk;

        DWORD typeFile;
        if (!FindChunk(pLock, sizeRes, fourccRIFF, &sizeChunk, &posChunk))
            throw "Failed to find RIFF chunk.";
        memcpy(&typeFile, pLock + posChunk, sizeof(DWORD));
        if (typeFile != fourccWAVE)
            throw "File type is different from WAVE.";

        WAVEFORMATEXTENSIBLE wfx;
        ZeroMemory(&wfx, sizeof(WAVEFORMATEXTENSIBLE));
        if (!FindChunk(pLock, sizeRes, fourccFMT, &sizeChunk, &posChunk))
            throw "Failed to find FMT  chunk.";
        memcpy(&wfx, pLock + posChunk, sizeChunk);

        if (!FindChunk(pLock, sizeRes, fourccDATA, &sizeChunk, &posChunk))
            throw "Failed to find DATA chunk.";
        BYTE* pDataBuffer = new BYTE[sizeChunk];
        memcpy(pDataBuffer, pLock + posChunk, sizeChunk);

        XAUDIO2_BUFFER bufXAudio;
        ZeroMemory(&bufXAudio, sizeof(XAUDIO2_BUFFER));
        bufXAudio.AudioBytes = sizeChunk;
        bufXAudio.pAudioData = pDataBuffer;
        bufXAudio.Flags = XAUDIO2_END_OF_STREAM;

        if(FAILED(pXAudio->CreateSourceVoice(&pAudio->pSVoice, (WAVEFORMATEX*)&wfx)))
            throw "Failed to create source voice.";

        if(FAILED(pAudio->pSVoice->SubmitSourceBuffer(&bufXAudio)))
            throw "Failed to submit source buffer.";

    } catch (const char* error) {
        MessageBoxA(nullptr, error, "Error", MB_OK);
        return false;
    }

    return true;
}
