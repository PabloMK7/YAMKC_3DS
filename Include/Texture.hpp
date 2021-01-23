#pragma once
#include <string>

class Texture
{
public:
	Texture(const std::string& fileName);
	~Texture();
	bool HasTransparency();
	bool IsLoaded();

	void Bind();

private:
	int textureId;
	bool isTransparent;
};