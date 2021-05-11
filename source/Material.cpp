#include "Material.hpp"

Material::Material() : Material("") {};

Material::Material(std::string name)
{
    this->name = name;
    isVisible = true;
    vArray = nullptr;
    texture = nullptr;
    SetBlendMode(BlendModeOpacity, nullptr);
    SetTexCombineMode(TexCombineModeNoTex, nullptr);
    SetFragmentMode(FragmentModeNone, nullptr);
    for (u32 i = 0; i < sizeof(constantColors) / sizeof(Color); i++)
        SetCostantColor(i, Color(1.f, 1.f, 1.f, 1.f));

    SetFragmentColor(FragmentColor::DIFFUSE, Color(1.f, 1.f, 1.f));
    SetFragmentColor(FragmentColor::AMBIENT, Color(0.f, 0.f, 0.f));
    SetFragmentColor(FragmentColor::SPECULAR0, Color(0.3f, 0.3f, 0.3f));
    SetFragmentColor(FragmentColor::SPECULAR1, Color(0.0f, 0.0f, 0.0f));
    SetFragmentColor(FragmentColor::EMISSION, Color(0.f, 0.f, 0.f));
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

Material& Material::AddFace(const Graphics::GPUFace& face)
{
    faces.push_back(face);
    return (*this);
}

Material& Material::SetTexture(const std::string& fileName, bool toVram)
{
    if (texture)
        delete texture;
    texture = new Texture(fileName, toVram);
    if (texture)
    {
        SetBlendMode(texture->IsTransparent() ? BlendModeTransparency : BlendModeOpacity, nullptr);
        SetTexCombineMode(TexCombineModeDeafult, nullptr);
    }
    return (*this);
}

Material& Material::SetCostantColor(u32 id, const Color& color)
{
    if (id < sizeof(constantColors) / sizeof(Color))
        constantColors[id] = color;
    return (*this);
}

Material& Material::SetFragmentColor(FragmentColor id, const Color& color) {
    fragmentColors[(u32)id] = color;
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

const Color& Material::GetFragmentColor(FragmentColor id) {
    return fragmentColors[(u32)id];
}

Graphics::VertexArray& Material::GetVertexArray()
{
    return *vArray;
}

Material& Material::SetVisible(bool visible)
{
    isVisible = visible;
    return (*this);
}

void Material::RunMaterialCallbacks()
{
    tMode(*this, texCombineUsrData);
    bMode(*this, blendUsrData);
    fMode(*this, fragModeUsrData);
}

void Material::Draw()
{
    if (!isVisible)
        return;
    
    if (texture)
    { // Modulate the vertex color with the texture
        texture->Bind();
    }
    RunMaterialCallbacks();
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
    env = C3D_GetTexEnv(2);
    C3D_TexEnvInit(env);
    if (mat.fMode != FragmentModeNone) {
        C3D_TexEnvSrc(env, C3D_RGB, GPU_FRAGMENT_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PREVIOUS); // (primary + secondary) * previous_operation
        C3D_TexEnvFunc(env, C3D_RGB, GPU_ADD_MULTIPLY);
    }
    
    for (int i = 3; i < 5; i++)
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

void Material::TexCombineModeFont(Material& mat, void* data) // Fonts only have alpha information
{
    C3D_TexEnv *env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_RGB, GPU_PRIMARY_COLOR, GPU_CONSTANT, GPU_CONSTANT); // RGB = vertex color * costant color (last arg not used)
    C3D_TexEnvSrc(env, C3D_Alpha, GPU_PRIMARY_COLOR, GPU_TEXTURE0, GPU_CONSTANT); // Alpha = vertex alpha * texture alpha (last arg not used)
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

static C3D_FVec g_lightVec;
static C3D_LightLut g_lut_Phong;

void Material::FragmentModeNone(Material& mat, void* data)
{
    C3D_LightEnvInit(&mat.lightEnv);
	C3D_LightEnvBind(&mat.lightEnv);
}

void Material::FragmentModePhong(Material& mat, void* data)
{
    C3D_LightEnvInit(&mat.lightEnv);
	C3D_LightEnvBind(&mat.lightEnv);
    C3D_Material c3dMat;
    c3dMat.ambient[2] = mat.fragmentColors[0].r;
    c3dMat.ambient[1] = mat.fragmentColors[0].g;
    c3dMat.ambient[0] = mat.fragmentColors[0].b;
    c3dMat.diffuse[2] = mat.fragmentColors[1].r;
    c3dMat.diffuse[1] = mat.fragmentColors[1].g;
    c3dMat.diffuse[0] = mat.fragmentColors[1].b;
    c3dMat.specular0[2] = mat.fragmentColors[2].r;
    c3dMat.specular0[1] = mat.fragmentColors[2].g;
    c3dMat.specular0[0] = mat.fragmentColors[2].b;
    c3dMat.specular1[2] = mat.fragmentColors[3].r;
    c3dMat.specular1[1] = mat.fragmentColors[3].g;
    c3dMat.specular1[0] = mat.fragmentColors[3].b;
    c3dMat.emission[2] = mat.fragmentColors[4].r;
    c3dMat.emission[1] = mat.fragmentColors[4].g;
    c3dMat.emission[0] = mat.fragmentColors[4].b;
    C3D_LightEnvMaterial(&mat.lightEnv, &c3dMat);
    C3D_LightEnvLut(&mat.lightEnv, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &g_lut_Phong);
    C3D_LightInit(&mat.light, &mat.lightEnv);
	C3D_LightColor(&mat.light, 1.0, 1.0, 1.0);
	C3D_LightPosition(&mat.light, &g_lightVec);
}

Material& Material::SetFragmentMode(FragmentMode mode, void* data) {
    fMode = mode;
    fragModeUsrData = data;
    return (*this);
}

void Material::InitFragmentLighting() {
    LightLut_Phong(&g_lut_Phong, 30);
    g_lightVec = FVec4_New(0.0f, -1.f, 0.f, 1.0f);
}