#include "../include/HeaderDX11.hpp"

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD* sizeChunk, DWORD* posChunkData) {
    if(SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD sizeRiffData = 0;
    DWORD bytesRead = 0;
    DWORD offset = 0;

    HRESULT hr = S_OK;
    while (hr == S_OK) {
        DWORD dwRead;

        DWORD typeChunk;
        if(ReadFile(hFile, &typeChunk, sizeof(DWORD), &dwRead, nullptr) == 0)
            hr = HRESULT_FROM_WIN32(GetLastError());

        DWORD sizeChunkTmp;
        if(ReadFile(hFile, &sizeChunkTmp, sizeof(DWORD), &dwRead, nullptr) == 0)
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (typeChunk) {
        case fourccRIFF:
            sizeRiffData = sizeChunkTmp;
            sizeChunkTmp = 4;
            DWORD typeFile;
            if(ReadFile(hFile, &typeFile, sizeof(DWORD), &dwRead, nullptr) == 0)
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;
        default:
            if(SetFilePointer(hFile, sizeChunkTmp, nullptr, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
            return HRESULT_FROM_WIN32(GetLastError());            
        }

        offset += sizeof(DWORD) * 2;

        if (typeChunk == fourcc) {
            *sizeChunk = sizeChunkTmp;
            *posChunkData = offset;
            return S_OK;
        }

        offset += sizeChunkTmp;

        if (sizeRiffData <= bytesRead)
            return S_FALSE;
    }

    return S_OK;
}

HRESULT ReadChunkData(HANDLE hFile, void* bufXAudio, DWORD sizeBuffer, DWORD offsetBuffer) {
    if(SetFilePointer(hFile, offsetBuffer, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if(ReadFile(hFile, bufXAudio, sizeBuffer, &dwRead, nullptr) == 0)
        return HRESULT_FROM_WIN32(GetLastError());
    return S_OK;
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
        HANDLE hFile = CreateFile("test.wav", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hFile == INVALID_HANDLE_VALUE)
            throw "Failed to create file handle.";

        if (SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
            throw "Failed to set file pointer.";

        DWORD sizeChunk;
        DWORD posChunk;

        DWORD typeFile;
        if (FAILED(FindChunk(hFile, fourccRIFF, &sizeChunk, &posChunk)))
            throw "Failed to find RIFF chunk.";
        if (FAILED(ReadChunkData(hFile, &typeFile, sizeof(DWORD), posChunk)))
            throw "Failed to read RIFF chunk data.";
        if (typeFile != fourccWAVE)
            throw "File type is different from WAVE.";

        WAVEFORMATEXTENSIBLE wfx;
        ZeroMemory(&wfx, sizeof(WAVEFORMATEXTENSIBLE));
        if (FAILED(FindChunk(hFile, fourccFMT, &sizeChunk, &posChunk)))
            throw "Failed to find FMT  chunk.";
        if (FAILED(ReadChunkData(hFile, &wfx, sizeChunk, posChunk)))
            throw "Failed to read FMT  chunk data.";

        if (FAILED(FindChunk(hFile, fourccDATA, &sizeChunk, &posChunk)))
            throw "Failed to find DATA chunk.";
        BYTE* pDataBuffer = new BYTE[sizeChunk];
        if (FAILED(ReadChunkData(hFile, pDataBuffer, sizeChunk, posChunk)))
            throw "Failed to read DATA chunk data.";

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
