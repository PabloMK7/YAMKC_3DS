#pragma once
#include <3ds.h>
#include <citro2d.h>
#include "Graphics.hpp"
#include "Vector.hpp"
#include "string"
#include "Color.hpp"
#include "Material.hpp"

class Text
{
public:
    static void Init();
    static void Terminate();

    Text() = delete; 
    Text(u32 maxChars);
    ~Text();

    void SetPosition(const Vector3& pos);
    void SetScale(const Vector2& scal);
    void SetText(const std::string& str);
    void SetColor(const Color& color, bool isTopColor);
    void SetBaseline(bool baseLineMode);
    void SetScreen(bool isTopScr);

    void Draw();

private:
    Graphics::VertexArray** vArray;
    Vector3 position = Vector3(0.f, 0.f, 0.f);
    Vector2 scale = Vector2(1.f, 1.f);
    bool atBaseLine = false;
    bool topScreen = true;
    std::string text;
    u32 maxCharacters;
    Color topColor = Color(1.f, 1.f, 1.f, 1.f);
    Color botColor = Color(1.f, 1.f, 1.f, 1.f);

    static C3D_Tex* glyphSheets;
    static Material* dummyMat;
};
