#pragma once
#include <3ds.h>
#include "Text.hpp"
#include "Fader.hpp"

class Chronometer
{
private:
    u32 frames;
    bool isPaused;
    Text text{9};
    Fader fade;
public:
    Chronometer();
    ~Chronometer();
    void Restart();
    void Tick();
    void Pause();
    void Play();
    int GetElapsed();
    void Draw();
};
