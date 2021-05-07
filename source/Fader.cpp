#include "Fader.hpp"

Fader::Fader() : Plane() {
    GetMaterial().SetTexCombineMode(Material::TexCombineModeNoTex, nullptr);
    GetMaterial().SetBlendMode(Material::BlendModeBlendedTransparency, nullptr);
    GetMaterial().SetCostantColor(0, Color(0.f, 0.f, 0.f, 0.f));
}
Fader::~Fader() {}

void Fader::SetColor(const Color& color) {
    Color setColor = GetMaterial().GetConstantColor(0);
    setColor.r = color.r; setColor.g = color.g; setColor.b = setColor.b;
    GetMaterial().SetCostantColor(0, setColor);
}
void Fader::SetFade(float amount) {
    Color setColor = GetMaterial().GetConstantColor(0);
    setColor.a = amount;
    GetMaterial().SetCostantColor(0, setColor);
}

void Fader::SetTargetFade(float amount, int frames) {
    targetFade = amount;
    targetFrame = frames;
    startFade = GetMaterial().GetConstantColor(0).a;
    currentFrame = targetFrame;
}

void Fader::Calc() {
    if (currentFrame == 0)
        return;
    float prog = currentFrame-- / (float)targetFrame;
    float newFade = startFade + (targetFade - startFade) * prog;
    SetFade(newFade);
}

