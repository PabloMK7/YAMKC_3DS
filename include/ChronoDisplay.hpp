#pragma once
#include <3ds.h>
#include <citro2d.h>
#include "Chronometer.hpp"

class ChronoDisplay
{
private:
    C2D_TextBuf g_dynamicBuf;
    u32 color;
    char chronoText[5];
public:
    ChronoDisplay(u32 color);
    ~ChronoDisplay();
    void Draw(C3D_RenderTarget* target, int w, int h);
    void SetElapsedTime(Chronometer* chronometer);
};
