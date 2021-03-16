#pragma once
#include "string"
#include "Color.hpp"
#include "Vector.hpp"

class Collision
{
public:
	enum class WallType {
		NONE,
		SOUTH,
		WEST,
		NORTH,
		EAST,
		ALL
	};
	Collision(std::string mapFile, std::string uiMapPosFile);
	~Collision();
	Color GetColorAtPixel(unsigned int x, unsigned int z);
	Color GetColorAtPosition(const Vector3& position);
	WallType GetWallTypeAtPosition(const Vector3& position);

private:
	void* image;
	unsigned int imageW;
	unsigned int imageH;
	float imageCoords[4]; // Bottom-Left, Top-Right
};
