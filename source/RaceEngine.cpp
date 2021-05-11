#include "RaceEngine.hpp"
#include "EndingDisplay.hpp"

extern int g_renderMode;
extern EndingDisplay* endingDisplay;

RaceEngine::RaceEngine(Kart* playerKart, Chronometer* chrono, CountdownDisplay* cntdwn, LapCounter* lapcnt) {
    currState = SequenceState::NONE;
    prevState = SequenceState::NONE;
    raceFader = new Fader();
    raceFader->GetScale() = Vector3(400.f, 240.f, 0.f);
    raceFader->GetPosition() = Vector3(200.f, 120.f, 0.85f);
    bgmEngine = new BgmEngine();
    count321 = new Sound("romfs:/audio/race/321.bcwav", 1);
    countGo = new Sound("romfs:/audio/race/go.bcwav", 1);
    secondLap = new Sound("romfs:/audio/race/2ndlap.bcwav", 1);
    finishRace = new Sound("romfs:/audio/race/finish.bcwav", 1);
    count321->SetMasterVolume(1.f);
    countGo->SetMasterVolume(1.f);
    secondLap->SetMasterVolume(1.25f);
    finishRace->SetMasterVolume(1.25f);
    kart = playerKart;
    chronometer = chrono;
    countdownDisp = cntdwn;
    lapCounterDisp = lapcnt;
    prevLap = 0;
}

RaceEngine::~RaceEngine() {
    delete raceFader;
    delete bgmEngine;
    delete count321;
    delete countGo;
    delete secondLap;
    delete finishRace;
}

void RaceEngine::Calc() {
    if (currState == SequenceState::NONE) {
        bgmEngine->PlayBGM(BgmEngine::BgmType::START);
        raceFader->SetTargetFade(0.f, 90);
        currState = SequenceState::PRECOUNTDOWN;
        countdownTimer = 60 * 3;
    }
    if (!countdownTimer) {
        if (currState == SequenceState::PRECOUNTDOWN) {
            currState = SequenceState::COUNTDOWN;
            countdownTimer = 60 * 3;
        }
    }
    if (countdownTimer && currState == SequenceState::COUNTDOWN) {
        if (countdownTimer == 60 * 3) {
            count321->Play();
            countdownDisp->SetText("3");
        } else if (countdownTimer == 60 * 2) {
            count321->Play();
            countdownDisp->SetText("2");
        } else if (countdownTimer == 60) {
            count321->Play();
            countdownDisp->SetText("1");
        }
    }
    if (!countdownTimer && currState == SequenceState::COUNTDOWN) {
        countGo->Play();
        currState = SequenceState::PRERACE;
        kart->EnableDrive(true);
        chronometer->Play();
        countdownDisp->SetText("GO!");
        countdownTimer = 30;
    }
    if (!countdownTimer && currState == SequenceState::PRERACE) {
        bgmEngine->PlayBGM(BgmEngine::BgmType::MAIN);
        currState = SequenceState::RACE;
    }

    int curLap = kart->GetLap();
    if ((currState == SequenceState::PRERACE || currState == SequenceState::RACE) && curLap > prevLap) {
        prevLap = curLap;
        lapCounterDisp->SetLap(curLap + 1);
        if (curLap == 1) {
            secondLap->Play();
        } else if (curLap == 2) {
            bgmEngine->PlayBGM(BgmEngine::BgmType::FASTMAIN);
        } else if (curLap == 3) {
            chronometer->Pause();
            kart->StopSounds();
            kart->EnableDrive(false);
            bgmEngine->StopBGM();
            finishRace->Play();
            countdownDisp->SetText("FINISH!");
            countdownTimer = 60;
            currState = SequenceState::FINISH;
        }
    }

    if (!countdownTimer && currState == SequenceState::FINISH) {
        bgmEngine->PlayBGM(BgmEngine::BgmType::FINISH);
        currState = SequenceState::END;
        raceFader->SetTargetFade(1.f, 120);
        countdownTimer = 121;
    }

    if (countdownTimer == 1 && currState == SequenceState::END) {
        endingDisplay->SetTime(chronometer->GetElapsed());
        raceFader->SetTargetFade(0.f, 120);
        g_renderMode = 1;
    }

    if (countdownTimer) countdownTimer--;
    bgmEngine->Tick();
    raceFader->Calc();
}

void RaceEngine::Draw() {
    raceFader->Draw();
}