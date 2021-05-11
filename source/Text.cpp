#include "Text.hpp"
#include "Graphics.hpp"
#include "Material.hpp"

C3D_Tex* Text::glyphSheets;
Material* Text::dummyMat;

void Text::Init() {
    TGLP_s          *glyphInfo;
    C3D_Tex         *tex;
    fontEnsureMapped();
    glyphInfo = fontGetGlyphInfo(NULL);
    glyphSheets = (C3D_Tex*)malloc(sizeof(C3D_Tex) * glyphInfo->nSheets);
    for (int i = 0; i < glyphInfo->nSheets; i++)
    {
        tex = &glyphSheets[i];
        tex->data = fontGetGlyphSheetTex(NULL, i);
        tex->fmt = (GPU_TEXCOLOR)glyphInfo->sheetFmt;
        tex->size = glyphInfo->sheetSize;
        tex->width = glyphInfo->sheetWidth;
        tex->height = glyphInfo->sheetHeight;
        tex->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)
            | GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_EDGE) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_EDGE);
    }
    dummyMat = new Material("");
    dummyMat->SetTexCombineMode(Material::TexCombineModeFont, nullptr);
    dummyMat->SetBlendMode(Material::BlendModeBlendedTransparency, nullptr);
}

void Text::Terminate() {
    free(glyphSheets);
    delete dummyMat;
}

Text::Text(u32 maxChars) {
    vArray = new Graphics::VertexArray*[maxChars];
    for (u32 j = 0; j < maxChars; j++)
    {
        vArray[j] = Graphics::VertexArray::Create();
        for (int i = 0; i < 4; i++)
        {
            Graphics::GPUVertex v;
            v.normal.x = 0.f;
            v.normal.y = 0.f;
            v.normal.z = 1.f;
            vArray[j]->AddVertex(v);
        }
        vArray[j]->Complete();
    }
    this->maxCharacters = maxChars;
}

Text::~Text() {
    for (u32 i = 0; i < maxCharacters; i++)
        Graphics::VertexArray::Dispose(vArray[i]);
    delete[] vArray;
}

void Text::SetPosition(const Vector3& pos) {
    position = pos;
}

void Text::SetScale(const Vector2& scal) {
    scale = scal;
}

void Text::SetText(const std::string& str) {
    text = str;
}

void Text::SetColor(const Color& color, bool isTopColor) {
    if (isTopColor)
        topColor = color;
    else
        botColor = color;
}

Color& Text::GetColor(bool isTopColor) {
    if (isTopColor)
        return topColor;
    else
        return botColor;
}

void Text::SetBaseline(bool baseLineMode) {
    atBaseLine = baseLineMode;
}

void Text::SetScreen(bool isTopScr) {
    topScreen = isTopScr;
}

void Text::GetTextSize(float& textX, float& textY) {
    const u8      *c;
    u32     code;
    ssize_t units;
    int     glyphIndex;
    fontGlyphPos_s  data;

    textX = 0.f;
    textY = 0.f;
    c = (const u8 *)text.c_str();

    do
    {
        if (!*c) break;
        units = decode_utf8(&code, c);
        if (units == -1) break;
        c += units;
        if (code > 0)
        {
            glyphIndex = fontGlyphIndexFromCodePoint(NULL, code);
            fontCalcGlyphPos(&data, NULL, glyphIndex, GLYPH_POS_CALC_VTXCOORD, scale.x, scale.y);
            if (data.vtxcoord.bottom > textY) textY = data.vtxcoord.bottom;
            textX += data.xAdvance;
        }
    } while (code > 0);
}

void Text::CenterH(bool isTopScreen) {
    float sX, sY;
    GetTextSize(sX, sY);
    float scrSize = isTopScreen ? 400.f : 320.f;
    position.x = (scrSize - sX) / 2.f;
}

void Text::Draw()
{
    u32             flags;
    u32             code;
    int             lastSheet;
    int             glyphIdx;
    u32             arrayIndex = 0;
    ssize_t         units;
    float           firstX;
    fontGlyphPos_s  data;
    const u8        *p = (const u8 *)text.c_str();

    float x = position.x;
    float y = position.y;
    float scaleX = scale.x;
    float scaleY = scale.y;

    firstX = x;
    flags = GLYPH_POS_CALC_VTXCOORD | (atBaseLine ? GLYPH_POS_AT_BASELINE : 0);
    lastSheet = -1;

    // Set up texture combiner and blending configs
    dummyMat->RunMaterialCallbacks();
    Graphics::UpdateModelViewMtx();

    do
    {
        if (!*p)
            break;
        units = decode_utf8(&code, p);
        if (units == -1)
            break;
        p += units;
        if (code == '\n')
        {
            x = firstX;
            y += scaleY * fontGetInfo(NULL)->lineFeed;
        }
        else if (code > 0)
        {
            glyphIdx = fontGlyphIndexFromCodePoint(NULL, code);
            fontCalcGlyphPos(&data, NULL, glyphIdx, flags, scaleX, scaleY);

            // Bind the correct texture sheet
            if (data.sheetIndex != lastSheet)
            {
                lastSheet = data.sheetIndex;
                C3D_TexBind(0, &glyphSheets[lastSheet]);
            }

            if (arrayIndex >= maxCharacters)
                break; // We can't render more characters

            // Add the vertices to the array
            Graphics::GPUVertex& leftBot = vArray[arrayIndex]->GetVertex(0);
            leftBot.position.x = x + data.vtxcoord.left;
            leftBot.position.y = y + data.vtxcoord.bottom;
            leftBot.position.z = position.z;
            leftBot.texcoord.u = data.texcoord.left;
            leftBot.texcoord.v = data.texcoord.bottom;
            leftBot.color.r = botColor.r; leftBot.color.g = botColor.g; leftBot.color.b = botColor.b; leftBot.color.a = botColor.a;
            Graphics::GPUVertex& rightBot = vArray[arrayIndex]->GetVertex(1);
            rightBot.position.x = x + data.vtxcoord.right;
            rightBot.position.y = y + data.vtxcoord.bottom;
            rightBot.position.z = position.z;
            rightBot.texcoord.u = data.texcoord.right;
            rightBot.texcoord.v = data.texcoord.bottom;
            rightBot.color.r = botColor.r; rightBot.color.g = botColor.g; rightBot.color.b = botColor.b; rightBot.color.a = botColor.a;
            Graphics::GPUVertex& leftTop = vArray[arrayIndex]->GetVertex(2);
            leftTop.position.x = x + data.vtxcoord.left;
            leftTop.position.y = y + data.vtxcoord.top;
            leftTop.position.z = position.z;
            leftTop.texcoord.u = data.texcoord.left;
            leftTop.texcoord.v = data.texcoord.top;
            leftTop.color.r = topColor.r; leftTop.color.g = topColor.g; leftTop.color.b = topColor.b; leftTop.color.a = topColor.a;
            Graphics::GPUVertex& rightTop = vArray[arrayIndex]->GetVertex(3);
            rightTop.position.x = x + data.vtxcoord.right;
            rightTop.position.y = y + data.vtxcoord.top;
            rightTop.position.z = position.z;
            rightTop.texcoord.u = data.texcoord.right;
            rightTop.texcoord.v = data.texcoord.top;
            rightTop.color.r = topColor.r; rightTop.color.g = topColor.g; rightTop.color.b = topColor.b; rightTop.color.a = topColor.a;

            // Draw the glyph
            vArray[arrayIndex]->Draw(GPU_TRIANGLE_STRIP);

            x += data.xAdvance;
            arrayIndex++;
        }
    } while (code > 0);
}