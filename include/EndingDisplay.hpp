#pragma once
#include "Text.hpp"

class EndingDisplay
{
private:
    Text yourTime{6};
    Text theTime{10};
    Text gameName{30};
    Text credits{50};
    Text pressToExit{50};
public:
    EndingDisplay();
    ~EndingDisplay();

    void SetTime(int frames);

    void DrawTop();
    void DrawBottom();
};
