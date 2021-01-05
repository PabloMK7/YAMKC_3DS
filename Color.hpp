#pragma once
// Clase que representa un color RGBA
class Color
{
public:
    Color() {
        this->r = this->g = this->b = this->a = 0;
    }
    Color(int r, int g, int b) {
        this->r = r / 255.f;
        this->g = g / 255.f;
        this->b = b / 255.f;
        this->a = 1;
    }
    Color(int r, int g, int b, int a) {
        this->r = r / 255.f;
        this->g = g / 255.f;
        this->b = b / 255.f;
        this->a = a / 255.f;
    }
    Color(float r, float g, float b) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = 1.f;
    }
    Color(float r, float g, float b, float a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    // No se ha usado esto al final, lo dejo en el código igualmente por si es necesario en el futuro
    // Gracias a este método, se pueden copiar colores en formato hexadecimal directamente de otras aplicaciones
    Color(unsigned int rgba) {
        this->r = ((rgba & 0xFF000000) >> 24) / 255.f;
        this->g = ((rgba & 0xFF0000) >> 16) / 255.f;
        this->b = ((rgba & 0xFF00) >> 8) / 255.f;
        this->a = (rgba & 0xFF) / 255.f;
    }

    void operator=(const Color& right)
    {
        this->r = right.r;
        this->g = right.g;
        this->b = right.b;
        this->a = right.a;
    }

    void Scale(const float& value, bool affectAlpha) {
        this->r *= value;
        this->g *= value;
        this->b *= value;
        if (affectAlpha) this->a *= value;
    }

    float r, g, b, a;
};
