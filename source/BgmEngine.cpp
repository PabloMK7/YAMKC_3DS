#include "BgmEngine.hpp"


BgmEngine::BgmEngine() {
    startBgm = new Sound("romfs:/audio/bgm/start.bcwav", 1);
    mainBgm = new Sound("romfs:/audio/bgm/main_theme.bcwav", 1);
    finishBgm = new Sound("romfs:/audio/bgm/finish.bcwav", 1);
    finalLapBgm = new Sound("romfs:/audio/bgm/final_lap.bcwav", 1);
    startBgm->SetVolume(1.f);
    mainBgm->SetVolume(0.75f);
    finishBgm->SetVolume(0.8f);
    finalLapBgm->SetVolume(0.8f);
}

BgmEngine::~BgmEngine() {
    StopBGM();
    delete startBgm;
    delete mainBgm;
    delete finishBgm;
    delete finalLapBgm;
}

void BgmEngine::PlayBGM(BgmEngine::BgmType bgm) {
    StopBGM();
    switch (bgm)
    {
    case BgmType::START:
        startBgm->StereoPlay();
        break;
    case BgmType::MAIN:
        mainBgm->StereoPlay();
        break;
    case BgmType::FASTMAIN:
        finalLapBgm->StereoPlay();
        fastVersionFrames = 60 * 3 + 30;
        break;
    case BgmType::FINISH:
        finishBgm->StereoPlay();
        break;
    default:
        break;
    }
}

void BgmEngine::StopBGM() {
    startBgm->Stop();
    mainBgm->Stop();
    finishBgm->Stop();
}

void BgmEngine::Tick() {
    if (fastVersionFrames) {
        if (!--fastVersionFrames) {
            mainBgm->SetPitch(1.1f, true);
            mainBgm->StereoPlay();
        }
    }
}