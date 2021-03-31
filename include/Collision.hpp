#pragma once
#include "string"
#include "Color.hpp"
#include "Vector.hpp"
#include "KCollisionServer.hpp"

class Collision
{
public:
	struct KCLAttr
	{
		union {
			u16 raw;
			struct
			{
				u16 type : 5;
				u16 basicEffect : 3;
				u16 shadow : 4;
				u16 flags : 4;
			};
		};
		
		KCLAttr(u16 val) : raw(val) { }
	};
	Collision(const std::string& kclFile);
	~Collision();
	CollisionResult GetAttributes(const Vector3& center, float radius, u32 maxCollisions);
	class KCLValueProperties
	{
	private:
		/* data */
	public:
		KCLAttr attr;
		float speedMultiplier = 1.f;
		bool isWall = false;

		KCLValueProperties(u16 kclAttr);
		
	};
	
private:
	KCollisionServer* server = nullptr;
	kcol_resource_t* kclBuff = nullptr;
};
