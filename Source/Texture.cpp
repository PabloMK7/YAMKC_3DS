#include "Texture.hpp"

unsigned int static loadImageFile(const char* nombre, bool& isTransparent)
{
	// Detecci�n del formato, lectura y conversion a BGRA
#pragma warning(disable:26812)
	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(nombre, 0);
#pragma warning(default:26812)
	FIBITMAP* imagen = FreeImage_Load(formato, nombre);
	if (imagen == nullptr)
		throw "File not found.";

	FIBITMAP* imagen32b = FreeImage_ConvertTo32Bits(imagen);
	isTransparent = FreeImage_IsTransparent(imagen32b);

	// Lectura de dimensiones y colores
	int w = FreeImage_GetWidth(imagen32b);
	int h = FreeImage_GetHeight(imagen32b);
	GLubyte* texeles = FreeImage_GetBits(imagen32b);

	// Activar texturas
	glEnable(GL_TEXTURE_2D);

	// Crear nueva textura
	unsigned int tId;
	glGenTextures(1, &tId);
	glBindTexture(GL_TEXTURE_2D, tId);

	// Carga como textura actual
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, texeles);

	// Liberar recursos
	FreeImage_Unload(imagen);
	FreeImage_Unload(imagen32b);

	//Opciones de textura
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tId;
}

Texture::Texture(const std::string& fileName)
{
	try
	{
		textureId = loadImageFile(fileName.c_str(), isTransparent);
	}
	catch (...)
	{
		textureId = -1;
	}
}

Texture::~Texture()
{
	if (textureId != -1)
		glDeleteTextures(1, (unsigned int*)&textureId);
}

bool Texture::HasTransparency()
{
	return isTransparent;
}

bool Texture::IsLoaded()
{
	return textureId != -1;
}

void Texture::Bind()
{
	if (textureId != -1)
		glBindTexture(GL_TEXTURE_2D, (unsigned int)textureId);
}
