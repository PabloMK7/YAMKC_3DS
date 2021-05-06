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

