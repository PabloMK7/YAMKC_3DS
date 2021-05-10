#pragma once
#include "Text.hpp"

class CountdownDisplay
{
private:
    Text text;
    int frameCounter;
public:
    CountdownDisplay(/* args */);
    ~CountdownDisplay();
    void SetText(const char* text);
    void Tick();
    void Draw();
};