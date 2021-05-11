#include "Texture.hpp"
#include <3ds.h>
#include <citro3d.h>
#include <tex3ds.h>

static bool textureHasAlpha(GPU_TEXCOLOR color) {
	switch (color)
	{
	case GPU_RGBA8:
	case GPU_RGBA5551:
	case GPU_RGBA4:
	case GPU_LA8:
	case GPU_A8:
	case GPU_LA4:
	case GPU_A4:
	case GPU_ETC1A4:
		return true;	
	default:
		return false;
	}
}

bool Texture::loadTextureFromFile(const char* fileName, C3D_Tex* texture, bool toVram)
{
	FILE* file = fopen(fileName, "rb");
	if(!file) return false;
	Tex3DS_Texture te3 = Tex3DS_TextureImportStdio(file, texture, NULL, toVram);
	if(!te3) return false;
	Tex3DS_TextureFree(te3);
	return true;
}

Texture::Texture(const std::string& fileName, bool toVram)
{
	correctyLoaded = loadTextureFromFile(fileName.c_str(), &textureData, toVram);
	isTransparent = textureHasAlpha(textureData.fmt);
	Wrap(GPU_REPEAT, GPU_REPEAT);
	C3D_TexSetFilter(&textureData, GPU_LINEAR, GPU_LINEAR);
}

Texture::~Texture()
{
	if (correctyLoaded)
		C3D_TexDelete(&textureData);
}

bool Texture::IsLoaded()
{
	return correctyLoaded;
}

bool Texture::IsTransparent()
{
	return isTransparent;
}

void Texture::Bind()
{
	if (correctyLoaded)
		C3D_TexBind(0, &textureData);
}

void Texture::Wrap(GPU_TEXTURE_WRAP_PARAM s, GPU_TEXTURE_WRAP_PARAM t) {
	if (correctyLoaded)
		C3D_TexSetWrap(&textureData, s, t);
}
