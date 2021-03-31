#include "Collision.hpp"
#include <iostream>
#include <fstream>

Collision::KCLValueProperties::KCLValueProperties(u16 val) : attr(val) {
	switch (attr.type)
	{
	case 0x0: // Road
		speedMultiplier = 1.f;
		isWall = false;
		break;
	case 0x5: // Off-road
		speedMultiplier = 0.5f;
		isWall = false;
		break;
	case 0x6: // Heavy Off-road
		speedMultiplier = 0.2f;
		isWall = false;
		break;
	case 0x10: // Wall type 1
		speedMultiplier = 1.f;
		isWall = true;
		break;
	default:
		break;
	}
}

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

CollisionResult Collision::GetAttributes(const Vector3& center, float radius, u32 maxCollisions)
{
	CollisionResult ret;
	server->CheckSphere(&ret, maxCollisions, center, radius, 1.f);
	return ret;
}


