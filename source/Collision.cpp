#include "Collision.hpp"
#include <iostream>
#include <fstream>

Collision::KCLValueProperties::KCLValueProperties(u16 val) : attr(val) {
	switch (attr.type)
	{
	case 0x0: // Road
		speedMultiplier = 1.f;
		isWall = false;
		roadType = 0;
		break;
	case 0x5: // Off-road
		speedMultiplier = 0.5f;
		isWall = false;
		roadType = 0x5;
		break;
	case 0x6: // Heavy Off-road
		speedMultiplier = 0.2f;
		isWall = false;
		roadType = 0x6;
		break;
	case 0x10: // Wall type 1
		speedMultiplier = 1.f;
		isWall = true;
		roadType = -1;
		break;
	default:
		break;
	}
}

Collision::Collision() {}

void Collision::AddResource(const std::string& kclFile, const Vector3& origin)
{
	FILE* f = fopen(kclFile.c_str(), "rb");
	fseek(f, 0, SEEK_END);
	u32 fSize = ftell(f);
	fseek(f, 0, SEEK_SET);
	kcol_resource_t* buf = (kcol_resource_t*)::operator new(fSize);
	fread(buf, 1, fSize, f);
	fclose(f);
	entities.push_back(new CollisionEntity(buf, origin));
}

Collision::~Collision()
{
	for (u32 i = 0; i < entities.size(); i++)
		delete entities[i];
	entities.clear();
}

CollisionResult Collision::CheckSphere(const Vector3& center, float radius)
{
	CollisionResult ret;
	for (u32 i = 0; i < entities.size(); i++) {
		CollisionResult curr;
		entities[i]->server->CheckSphere(&curr, MAX_NR_OF_COLLISIONS - ret.length, center - entities[i]->origin, radius, 1.f, i);
		ret.Merge(curr);
	}
	return ret;
}


