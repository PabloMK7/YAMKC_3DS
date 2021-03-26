#include "Collision.hpp"
#include <iostream>
#include <fstream>

Collision::Collision(const std::string& kclFile)
{
	FILE* f = fopen(kclFile.c_str(), "rb");
	fseek(f, 0, SEEK_END);
	u32 fSize = ftell(f);
	fseek(f, 0, SEEK_SET);
	kclBuff = (kcol_resource_t*)::operator new(fSize);
	fread(kclBuff, 1, fSize, f);
	fclose(f);
	server = new KCollisionServer(kclBuff);
}

Collision::~Collision()
{
	if (server) delete server;
	if (kclBuff) ::operator delete(kclBuff);
}

u16 Collision::GetAttributAtPos(const Vector3& pos)
{
	CollisionResult res; 
	server->CheckSphere(&res, 1, pos, 1.f, 1.f);
	if (res.length > 0)
		return res.prisms[0]->attribute;
	else
		return 0xFFFF;
}

Color Collision::GetColorAtPixel(unsigned int x, unsigned int z)
{

	/*RGBQUAD col;
	FreeImage_GetPixelColor((FIBITMAP*)image, x, imageH - z, &col);
	return Color(col.rgbRed, col.rgbGreen, col.rgbBlue);*/
	return Color(0, 0, 0);
}

Color Collision::GetColorAtPosition(const Vector3& position)
{
	return Color(0, 0, 0);
}

Collision::WallType Collision::GetWallTypeAtPosition(const Vector3& position)
{
	Color c = GetColorAtPosition(position);
	float blue = c.b;
	if (blue < 0.1f)
		return WallType::NONE;
	else if (blue > 0.2 && blue < 0.35)
		return WallType::SOUTH;
	else if (blue >= 0.35 && blue < 0.65)
		return WallType::WEST;
	else if (blue >= 0.65 && blue < 0.85)
		return WallType::NORTH;
	else if (blue >= 0.85)
		return WallType::EAST;
	else
		return WallType::ALL;
}

