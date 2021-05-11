#pragma once
#include "Graphics.hpp"
#include "Texture.hpp"
#include "Color.hpp"
#include <tuple>

class Material
{
public:
    enum class FragmentColor {
        AMBIENT = 0,
        DIFFUSE = 1,
        SPECULAR0 = 2,
        SPECULAR1 = 3,
        EMISSION = 4
    };
    Material();
    Material(std::string name);
    ~Material();
    std::string& GetName();
    Material& AddFace(const Graphics::GPUFace& face);
    Material& SetTexture(const std::string& fileName, bool toVram = true);
    Material& SetCostantColor(u32 id, const Color& color);
    Material& SetFragmentColor(FragmentColor id, const Color& color);
    const Color& GetConstantColor(u32 id);
    const Color& GetFragmentColor(FragmentColor id);
    Graphics::VertexArray& GetVertexArray();
    Material& SetVisible(bool visible);
    void RunMaterialCallbacks();
    void Draw();

    inline Material& SetTextureWrapMode(GPU_TEXTURE_WRAP_PARAM sWrap, GPU_TEXTURE_WRAP_PARAM tWrap) {texture->Wrap(sWrap, tWrap); return *this;}

    using BlendMode = void(*)(Material&, void*);
    using TexCombineMode = void(*)(Material&, void*);
    using FragmentMode = void(*)(Material&, void*);

    static void BlendModeOpacity(Material& mat, void* data);
    static void BlendModeTransparency(Material& mat, void* data);
    static void BlendModeBlendedTransparency(Material& mat, void* data);
    static void BlendModeAdditiveTransparency(Material& mat, void* data);

    Material& SetBlendMode(BlendMode mode, void* data);

    static void TexCombineModeDeafult(Material& mat, void* data);
    static void TexCombineModeNoTex(Material& mat, void* data);
    static void TexCombineModeFont(Material& mat, void* data);

    Material& SetTexCombineMode(TexCombineMode mode, void* data);

    static void FragmentModeNone(Material& mat, void* data);
    static void FragmentModePhong(Material& mat, void* data);

    Material& SetFragmentMode(FragmentMode mode, void* data);
    static void InitFragmentLighting();

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
    FragmentMode fMode;
    void* fragModeUsrData;
    Color constantColors[5];
    Color fragmentColors[5];
    C3D_LightEnv lightEnv;
    C3D_Light light;
};