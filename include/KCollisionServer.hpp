#pragma once
#include "3ds.h"
#include "Vector.hpp"

#define MAX_NR_OF_COLLISIONS 8

enum class KCHitSphereClassification {
	None = 0,
	Plane = 1,
	Edge1 = 2,
	Edge2 = 3,
	Edge3 = 4,
	Vertex1 = 5,
	Vertex2 = 6,
	Vertex3 = 7,
};

typedef struct
{
	float height;
	u16 posIdx;
	u16 fNrmIdx;
	u16 eNrm1Idx;
	u16 eNrm2Idx;
	u16 eNrm3Idx;
	u16 attribute;
} kcol_prism_data_t;

static_assert(sizeof(kcol_prism_data_t) == 0x10);

typedef struct
{
	float* posDataOffset;
	float* nrmDataOffset;
	kcol_prism_data_t* prismDataOffset;
	u32* blockDataOffset;
	float prismThickness;
	float areaMinPosx;
	float areaMinPosy;
	float areaMinPosz;
	u32 areaXWidthMask;
	u32 areaYWidthMask;
	u32 areaZWidthMask;
	u32 blockWidthShift;
	u32 areaXBlocksShift;
	u32 areaXYBlocksShift;
	float sphereRadius;
} kcol_header_t;

static_assert(sizeof(kcol_header_t) == 0x3C);

struct kcol_resource_t
{
	kcol_header_t header;
};

struct KC_PrismHit
{
	float distance;
	KCHitSphereClassification classification;
};

class CollisionResult
{
public:
	kcol_prism_data_t* prisms[MAX_NR_OF_COLLISIONS];
	float distances[MAX_NR_OF_COLLISIONS];
	KCHitSphereClassification classifications[MAX_NR_OF_COLLISIONS];
	u32 length;

	void Reset()
	{
		length = 0;
	}
};

class KCollisionServer
{
private:
	const kcol_header_t* _kcl;
	
	float* _positions;
	float* _normals;
	kcol_prism_data_t* _prisms;
	u32* _blockData;

	bool IsInsideMinMaxInLocalSpace(const Vector3& pos);
	float CalculateVertexSquareDistance(float dot1, float dot2, float dot3, const Vector3& a, const Vector3& b);
	bool KCHitArrow(KC_PrismHit& dst, const kcol_prism_data_t* plane, const Vector3& origin, const Vector3& arrowDir);
	bool KCHitSphere(KC_PrismHit& dst, const kcol_prism_data_t* plane, const Vector3& position, float radius, float thickness);

public:
	KCollisionServer(const kcol_resource_t* kcl);

	/**
	 * \brief Shoots an arrow through the mesh and returns all hit results.
	 */
	bool CheckArrow(CollisionResult* result, u32 maxCollisions, const Vector3& origin, const Vector3& direction, float length);

	/**
	 * \brief Checks the mesh against a sphere and returns all hit results.
	 */
	bool CheckSphere(CollisionResult* result, u32 maxCollisions, const Vector3& position, float radius, float scale);

	inline Vector3 GetNormal(u32 idx) const { idx *= 0x3; return Vector3(_normals[idx], _normals[idx + 1], _normals[idx + 2]);}
	inline Vector3 GetPosition(u32 idx) const { idx *= 0x3; return Vector3(_positions[idx], _positions[idx + 1], _positions[idx + 2]);}
};