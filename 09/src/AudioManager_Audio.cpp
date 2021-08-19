#include "../include/HeaderDX11.hpp"

Audio::Audio() : pSVoice(nullptr) {}

Audio::~Audio() {
    if (pSVoice != nullptr)
        pSVoice->DestroyVoice();
}

void Audio::play() {
    pSVoice->Start(0);
}

void Audio::stop() {
    pSVoice->Stop();
}
