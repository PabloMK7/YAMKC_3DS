#include "Material.hpp"


Material::Material(std::string name)
{
    this->name = name;
    isVisible = true;
    vArray = nullptr;
    SetBlendMode(BlendModeOpacity, nullptr);
    SetTexCombineMode(TexCombineModeNoTex, nullptr);
    for (u32 i = 0; i < sizeof(constantColors) / sizeof(Color); i++)
        SetCostantColor(i, Color(1.f, 1.f, 1.f, 1.f));
}

Material::~Material()
{
    if (texture)
        delete texture;
    if (vArray)
        Graphics::VertexArray::Dispose(vArray);
}

std::string& Material::GetName()
{
    return name;
}

void Material::AddFace(const Graphics::GPUFace& face)
{
    faces.push_back(face);
}

void Material::SetTexture(const std::string& fileName)
{
    if (texture)
        delete texture;
    texture = new Texture(fileName);
    if (texture)
    {
        SetBlendMode(texture->IsTransparent() ? BlendModeTransparency : BlendModeOpacity, nullptr);
        SetTexCombineMode(TexCombineModeDeafult, nullptr);
    }
}

Material& Material::SetCostantColor(u32 id, const Color& color)
{
    if (id < sizeof(constantColors) / sizeof(Color))
        constantColors[id] = color;
    return (*this);
}

static Color defaultColor;
const Color& Material::GetConstantColor(u32 id)
{
    if (id < sizeof(constantColors) / sizeof(Color))
        return constantColors[id];
    else
        return defaultColor;
}

void Material::SetVisible(bool visible)
{
    isVisible = visible;
}

void Material::Draw()
{
    if (!isVisible)
        return;
    
    if (texture)
    { // Modulate the vertex color with the texture
        texture->Bind();
    }
    tMode(*this, texCombineUsrData);
    bMode(*this, blendUsrData);
    vArray->Draw(GPU_TRIANGLES);
}

void Material::ConvertToVBO(void)
{
    vArray = Graphics::VertexArray::Create();
    for (auto it = faces.cbegin(); it != faces.cend(); it++) {
        vArray->AddVertex(std::get<0>(*it));
        vArray->AddVertex(std::get<1>(*it));
        vArray->AddVertex(std::get<2>(*it));
    }
    vArray->Complete();
    faces.clear();
}

void Material::BlendModeOpacity(Material& mat, void* data)
{
    C3D_FragOpMode(GPU_FRAGOPMODE_GL);
    C3D_ColorLogicOp(GPU_LOGICOP_COPY); // Is this the proper way to disable alpha blend?
    C3D_AlphaTest(false, GPU_ALWAYS, 0);
    C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);
}

void Material::BlendModeTransparency(Material& mat, void* data)
{
    C3D_FragOpMode(GPU_FRAGOPMODE_GL);
    C3D_ColorLogicOp(GPU_LOGICOP_COPY); // Is this the proper way to disable alpha blend?
    C3D_AlphaTest(true, GPU_GREATER, 0x7F);
    C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);
}

void Material::BlendModeBlendedTransparency(Material& mat, void* data)
{
    C3D_FragOpMode(GPU_FRAGOPMODE_GL);
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_ZERO, GPU_ZERO);
    C3D_AlphaTest(false, GPU_ALWAYS, 0);
    C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_COLOR);
}

void Material::BlendModeAdditiveTransparency(Material& mat, void* data)
{
    C3D_FragOpMode(GPU_FRAGOPMODE_GL);
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE, GPU_ZERO, GPU_ZERO);
    C3D_AlphaTest(false, GPU_ALWAYS, 0);
    C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_COLOR);
}

Material& Material::SetBlendMode(BlendMode mode, void* data)
{
    bMode = mode;
    blendUsrData = data;
    return *this;
}

void Material::TexCombineModeDeafult(Material& mat, void* data)
{
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_CONSTANT, GPU_CONSTANT); // vertex color * costant_color (last arg not used)
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
    C3D_TexEnvColor(env, mat.GetConstantColor(0).AsABGR());
    env = C3D_GetTexEnv(1);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PREVIOUS, GPU_TEXTURE0, GPU_CONSTANT); // previous_operation * texture0 (last arg not used)
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
    
    for (int i = 2; i < 5; i++)
        C3D_TexEnvInit(C3D_GetTexEnv(i));
}

void Material::TexCombineModeNoTex(Material& mat, void* data)
{
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_CONSTANT, GPU_CONSTANT); // vertex color * costant_color (last arg not used)
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
    C3D_TexEnvColor(env, mat.GetConstantColor(0).AsABGR());
    
    for (int i = 1; i < 5; i++)
        C3D_TexEnvInit(C3D_GetTexEnv(i));
}

Material& Material::SetTexCombineMode(TexCombineMode mode, void* data)
{
    tMode = mode;
    texCombineUsrData = data;
    return (*this);
}