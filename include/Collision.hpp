#pragma once
#include "string"
#include "Color.hpp"
#include "Vector.hpp"
#include "KCollisionServer.hpp"
#include "vector"

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

	Collision();
	void AddResource(const std::string& file, const Vector3& origin, const Angle3& rotation);
	~Collision();
	CollisionResult CheckSphere(const Vector3& center, float radius);
	inline Vector3 GetNormal(u32 normalIndex, u32 serverID) {return entities[serverID]->server->GetNormal(normalIndex);}
	class KCLValueProperties
	{
	private:
		/* data */
	public:
		KCLAttr attr;
		float speedMultiplier = 1.f;
		int checkpointID = -1;
		bool isWall = false;
		int roadType;

		KCLValueProperties(u16 kclAttr);
		
	};
	
private:
	class CollisionEntity {
		public:
			KCollisionServer* server;
			kcol_resource_t* kclRes;
			Vector3 origin;
			Angle3 rotation;
			
			CollisionEntity(const std::string& kclFile, Vector3 orig, Angle3 rot) : origin(orig), rotation(rot) {
				FILE* f = fopen(kclFile.c_str(), "rb");
				fseek(f, 0, SEEK_END);
				u32 fSize = ftell(f);
				fseek(f, 0, SEEK_SET);
				kclRes = (kcol_resource_t*)::operator new(fSize);
				fread(kclRes, 1, fSize, f);
				fclose(f);
				server = new KCollisionServer(kclRes);
			}
			~CollisionEntity() {
				delete server;
				::operator delete(kclRes);
			}
	};
	std::vector<CollisionEntity*> entities;
};
