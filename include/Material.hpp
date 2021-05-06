#pragma once
#include "Graphics.hpp"
#include "Texture.hpp"
#include "Color.hpp"
#include <tuple>

class Material
{
public:
    
    Material();
    Material(std::string name);
    ~Material();
    std::string& GetName();
    Material& AddFace(const Graphics::GPUFace& face);
    Material& SetTexture(const std::string& fileName);
    Material& SetCostantColor(u32 id, const Color& color);
    const Color& GetConstantColor(u32 id);
    Material& SetVisible(bool visible);
    void RunMaterialCallbacks();
    void Draw();

    inline void SetTextureWrapMode(GPU_TEXTURE_WRAP_PARAM sWrap, GPU_TEXTURE_WRAP_PARAM tWrap) {texture->Wrap(sWrap, tWrap);}

    using BlendMode = void(*)(Material&, void*);
    using TexCombineMode = void(*)(Material&, void*);

    static void BlendModeOpacity(Material& mat, void* data);
    static void BlendModeTransparency(Material& mat, void* data);
    static void BlendModeBlendedTransparency(Material& mat, void* data);
    static void BlendModeAdditiveTransparency(Material& mat, void* data);

    Material& SetBlendMode(BlendMode mode, void* data);

    static void TexCombineModeDeafult(Material& mat, void* data);
    static void TexCombineModeNoTex(Material& mat, void* data);
    static void TexCombineModeFont(Material& mat, void* data);

    Material& SetTexCombineMode(TexCombineMode mode, void* data);

    void ConvertToVBO(void);
friend class Obj;
private:
    Texture* texture;
    std::string name;
    std::vector<Graphics::GPUFace> faces;
    bool isVisible;
    Graphics::VertexArray* vArray;
    BlendMode bMode;
    void* blendUsrData;
    TexCombineMode tMode;
    void* texCombineUsrData;
    Color constantColors[5];

    
};