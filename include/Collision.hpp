#pragma once
#include "string"
#include "Color.hpp"
#include "Vector.hpp"
#include "KCollisionServer.hpp"

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
	Collision(const std::string& kclFile);
	~Collision();
	u16 GetAttributAtPos(const Vector3& pos);

	Color GetColorAtPixel(unsigned int x, unsigned int z);
	Color GetColorAtPosition(const Vector3& position);
	WallType GetWallTypeAtPosition(const Vector3& position);

private:
	KCollisionServer* server = nullptr;
	kcol_resource_t* kclBuff = nullptr;
};
