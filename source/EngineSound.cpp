#include "EngineSound.hpp"
#include <iostream>
#include "math.h"

EngineSound::EngineSound(/* args */) :
    idleSound("romfs:/audio/kart/engine/idle.bcwav", 1),
    startSound("romfs:/audio/kart/engine/start.bcwav", 1),
    runSound("romfs:/audio/kart/engine/run.bcwav", 1),
    endSound("romfs:/audio/kart/engine/end.bcwav",1)
{
    speed = 0.f;
    prevSpeed = 0.f;
    idleSound.SetMasterVolume(0.95f);
    startSound.SetMasterVolume(0.95f);
    runSound.SetMasterVolume(0.85f);
    endSound.SetMasterVolume(0.95f);
    idleSound.SetVolume(0.f);
    startSound.SetVolume(0.f);
    runSound.SetVolume(0.f);
    endSound.SetVolume(0.f);
    idleSound.Play();
    runSound.Play();
    state = StateMachine::NONE;
    prevState = StateMachine::NONE;
}

EngineSound::~EngineSound()
{
}

void EngineSound::CalcStateMachine() {
    if (state == prevState)
        return;
    if (state == StateMachine::IDLE) {
        startSound.SetTargetVolume(0.f, 10);
        startSound.SetTargetStop(10);
        idleSound.SetTargetVolume(1.f, 30);
        endSound.SetTargetStop(60);
        endSound.SetTargetVolume(0.f, 60);
        runSound.SetTargetVolume(0.f, 30);
    } else if (state == StateMachine::START) {
        startSound.Stop();
        startSound.Play();
        startSound.SetTargetVolume(1.f, 1);
        startSound.SetDecayPitch(1.f, 60);
        idleSound.SetTargetVolume(0.4f, 30);
        endSound.SetTargetVolume(0.f, 10);
        endSound.SetTargetStop(10);
        runSound.SetTargetVolume(0.f, 10);
    } else if (state == StateMachine::DECEL) {
        startSound.SetTargetStop(20);
        startSound.SetTargetVolume(0.f, 20);
        endSound.Stop();
        endSound.Play();
        endSound.SetCreatePitch(0.7f, 90);
        endSound.SetTargetVolume(1.f, 10);
        idleSound.SetTargetVolume(1.f, 120);
        runSound.SetTargetVolume(0.f, 10);
    } else if (state == StateMachine::FAST) {
        endSound.SetTargetStop(10);
        endSound.SetTargetVolume(0.f, 10);
        startSound.SetTargetStop(150);
        startSound.SetTargetVolume(0.f, 150);
        runSound.SetTargetVolume(1.f, 60);
        idleSound.SetTargetVolume(0.f, 60);
    }
    
    prevState = state;
}

void EngineSound::Calc(float newSpeed)
{
    bool isBack = newSpeed < 0.f;
    newSpeed = fabsf(newSpeed);
    bool accelerating = newSpeed > prevSpeed;

    if (state == StateMachine::NONE) {
        state = StateMachine::IDLE;
    }

    if (!accelerating && newSpeed < 10.f) {
        state = StateMachine::IDLE;
    } else if ((accelerating && newSpeed < 10.f) || (state == StateMachine::DECEL && accelerating)) {
        state = StateMachine::START;
    } else if (!accelerating && newSpeed > 10.f && newSpeed < 19.f && !isBack) {
        state = StateMachine::DECEL;
    } else if ((newSpeed > 10.f && accelerating) || newSpeed >= 19.f || (isBack && newSpeed > 2.5f)) {
        state = StateMachine::FAST;
    }

    float pitchBend = (newSpeed - 10.f) / (65.f - 10.f);
    if (pitchBend < 0.f)
        pitchBend = 0.f;
    else if (pitchBend > 1.f)
        pitchBend = 1.f;

    float pitchAmount = 0.65f + pitchBend * 0.45f;
    targetPitch = pitchAmount;
    runSound.SetPitch(targetPitch, true);
    startSound.SetPitch(targetPitch);
    endSound.SetPitch(targetPitch);

    idleSound.Tick();
    startSound.Tick();
    runSound.Tick();
    endSound.Tick();

    CalcStateMachine();

    prevSpeed = newSpeed;
}

void EngineSound::Terminate() {
    idleSound.SetMasterVolume(0.f);
    startSound.SetMasterVolume(0.f);
    runSound.SetMasterVolume(0.f);
    endSound.SetMasterVolume(0.f);
}