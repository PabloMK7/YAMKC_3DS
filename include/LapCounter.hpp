#pragma once
#include <3ds.h>
#include "Text.hpp"
#include "Fader.hpp"

class LapCounter
{
private:
    Text text{10};
    Fader fade;
    int currLap = 1;
public:
    LapCounter();
    ~LapCounter();
    void SetLap(int lap);
    void Draw();
};
