#include "Texture.hpp"
#include <3ds.h>
#include <citro3d.h>
#include <tex3ds.h>

static bool loadTextureFromFile(const char* fileName, C3D_Tex* texture)
{
	FILE* file = fopen(fileName, "rb");
	if(!file) return false;
	Tex3DS_Texture te3 = Tex3DS_TextureImportStdio(file, texture, NULL, false);
	if(!te3) return false;
	Tex3DS_TextureFree(te3);
	C3D_TexSetFilter(texture, GPU_LINEAR, GPU_NEAREST);
	return true;
}

Texture::Texture(const std::string& fileName)
{
	correctyLoaded = loadTextureFromFile(fileName.c_str(), &textureData);
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

void Texture::Bind()
{
	if (correctyLoaded)
		C3D_TexBind(0, &textureData);
}

void Texture::Wrap(GPU_TEXTURE_WRAP_PARAM s, GPU_TEXTURE_WRAP_PARAM t) {
	if (correctyLoaded)
		C3D_TexSetWrap(&textureData, s, t);
}
