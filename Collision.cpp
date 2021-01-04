#include "Collision.hpp"
#include <iostream>
#include <fstream>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include <freeimage/FreeImage.h>

Collision::Collision(std::string mapfile, std::string uiMapPosFile)
{
	// Detección del formato, lectura y conversion a BGRA
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(mapfile.c_str(), 0);
	image = (FIBITMAP*)FreeImage_Load(format, mapfile.c_str());
	if (image == nullptr)
		throw "Collision file not found.";

	imageW = FreeImage_GetWidth((FIBITMAP*)image);
	imageH = FreeImage_GetHeight((FIBITMAP*)image);

	std::ifstream myFile;
	try
	{
		myFile.open(uiMapPosFile, std::ios::binary);
	}
	catch (...){
		throw "UIMapPos file not found.";
	}
	myFile.seekg(4 * sizeof(float), myFile.beg);
	myFile.read((char*)imageCoords, sizeof(imageCoords));
}

Collision::~Collision()
{
	FreeImage_Unload((FIBITMAP*)image);
}

Color Collision::GetColorAtPixel(unsigned int x, unsigned int z)
{
	if (x >= imageW)
		x = imageW - 1;
	if (z >= imageW)
		z = imageW - 1;

	RGBQUAD col;
	FreeImage_GetPixelColor((FIBITMAP*)image, x, imageH - z, &col);
	return Color(col.rgbRed, col.rgbGreen, col.rgbBlue);
}

Color Collision::GetColorAtPosition(const Vector3& position)
{
	float minX = imageCoords[0];
	float maxZ = imageCoords[1];
	float maxX = imageCoords[2];
	float minZ = imageCoords[3];

	unsigned int imageX = (unsigned int)(((position.x - minX) / (maxX - minX)) * imageW);
	unsigned int imageZ = (unsigned int)(((position.z - minZ) / (maxZ - minZ)) * imageH);

	return GetColorAtPixel(imageX, imageZ);
}
