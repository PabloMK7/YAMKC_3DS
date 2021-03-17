#pragma once
#include <string>
#include <3ds.h>
#include <citro3d.h>
#include <tex3ds.h>

class Texture
{
public:
	Texture(const std::string& fileName);
	~Texture();
	bool IsLoaded();
	bool IsTransparent();
	void Bind();
	void Wrap(GPU_TEXTURE_WRAP_PARAM s, GPU_TEXTURE_WRAP_PARAM t);

private:
	C3D_Tex textureData;
	bool isTransparent;
	bool correctyLoaded;
	bool loadTextureFromFile(const char* fileName, C3D_Tex* texture); 
};