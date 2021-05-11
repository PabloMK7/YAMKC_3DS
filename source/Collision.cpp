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
	case 0x0A: // Checkpoint
		checkpointID = attr.shadow;
		break;
	default:
		break;
	}
}

Collision::Collision() {}

void Collision::AddResource(const std::string& kclFile, const Vector3& origin, const Angle3& rotation)
{
	
	entities.push_back(new CollisionEntity(kclFile, origin, rotation));
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
	CollisionResult curr;
	for (u32 i = 0; i < entities.size(); i++) {
		Vector3 checkPoint = center;
		checkPoint.Rotate(entities[i]->rotation * -1.f, entities[i]->origin);
		checkPoint -= entities[i]->origin;
		entities[i]->server->CheckSphere(&curr, MAX_NR_OF_COLLISIONS - ret.length, checkPoint, radius, 1.05f, i);
		ret.Merge(curr);
		curr.Reset();
	}
	return ret;
}


