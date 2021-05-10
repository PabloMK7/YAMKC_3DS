#pragma once
#include "Plane.hpp"
#include "Vector.hpp"
#include "Material.hpp"

class Fader : public Plane
{
private:
    float targetFade;
    int targetFrame = 0;
    int currentFrame;
    float startFade;
public:
    Fader();
    ~Fader();
    void SetColor(const Color& color);
    void SetFade(float amount);
    void SetTargetFade(float amount, int frames);
    void Calc();
};
