#include "KCollisionServer.hpp"

KCollisionServer::KCollisionServer(const kcol_resource_t* kcl)
    : _kcl(&kcl->header)
{
    _positions = (float*)((u8*)_kcl + (u32)_kcl->posDataOffset);
    _normals = (float*)((u8*)_kcl + (u32)_kcl->nrmDataOffset);
    _prisms = (kcol_prism_data_t*)((u8*)_kcl + (u32)_kcl->prismDataOffset);
    _blockData = (u32*)((u8*)_kcl + (u32)_kcl->blockDataOffset);
}


bool KCollisionServer::IsInsideMinMaxInLocalSpace(const Vector3& pos)
{
    return
        ((int)pos.x & _kcl->areaXWidthMask) == 0 &&
        ((int)pos.y & _kcl->areaYWidthMask) == 0 &&
        ((int)pos.z & _kcl->areaZWidthMask) == 0;
}

float KCollisionServer::CalculateVertexSquareDistance(float dot1, float dot2, float dot3, const Vector3& a, const Vector3& b)
{
    float aFactor = (dot1 * dot2 - dot3) / ( dot1 * dot1 - 1.f);
    float bFactor = dot2 - (aFactor * dot1);
    Vector3 result = (a * aFactor) + (b * bFactor);
    return result.MagNoSqrt();
}

bool KCollisionServer::KCHitArrow(KC_PrismHit& dst, const kcol_prism_data_t* prism, const Vector3& origin, const Vector3& arrowDir)
{
    Vector3 scratchVec3c = GetNormal(prism->fNrmIdx);

    // Local space.
    Vector3 scratchVec3d = origin - GetPosition(prism->posIdx);

    float proj = scratchVec3c.Dot(scratchVec3d);
    if (proj < 0)
        return false;

    float projDir = scratchVec3c.Dot(arrowDir);
    if (proj + projDir >= 0)
        return false;

    float dist = proj / -projDir;
    scratchVec3c = scratchVec3d + (arrowDir * dist);

    scratchVec3d = GetNormal(prism->eNrm1Idx);
    float dotNrm1 = scratchVec3c.Dot(scratchVec3d);
    if (dotNrm1 >= 0.01f)
        return false;

    scratchVec3d = GetNormal(prism->eNrm2Idx);
    float dotNrm2 = scratchVec3c.Dot(scratchVec3d);
    if (dotNrm2 >= 0.01f)
        return false;

    scratchVec3d = GetNormal(prism->eNrm3Idx);
    float dotNrm3 = scratchVec3c.Dot(scratchVec3d);
    if (dotNrm3 >= 0.01f + prism->height)
        return false;

    dst.distance = dist;

    return true;
}

bool KCollisionServer::KCHitSphere(KC_PrismHit& dst, const kcol_prism_data_t* prism, const Vector3& position, float radius, float thickness)
{
    Vector3 scratchVec3d = position - GetPosition(prism->posIdx);

    Vector3 scratchVec3c = GetNormal(prism->eNrm1Idx);
    float dotNrm1 = scratchVec3c.Dot(scratchVec3d);
    if (dotNrm1 >= radius)
        return false;

    scratchVec3c = GetNormal(prism->eNrm2Idx);
    float dotNrm2 = scratchVec3c.Dot(scratchVec3d);
    if (dotNrm2 >= radius)
        return false;

    scratchVec3c = GetNormal(prism->eNrm3Idx);
    float dotNrm3 = scratchVec3c.Dot(scratchVec3d) - prism->height;
    if (dotNrm3 >= radius)
        return false;

    scratchVec3c = GetNormal(prism->fNrmIdx);
    float dotFaceNrm = scratchVec3c.Dot(scratchVec3d);
    if (radius - dotFaceNrm < 0.f)
        return false;

    float sqRadius = radius * radius;
    float distance;

    dst.classification = None;

    float t1 = INFINITY;
    if (dotNrm1 >= dotNrm2 && dotNrm1 >= dotNrm3 && dotNrm1 >= 0.f)
    {
        // dotNrm1 is the maximum. Pick other edge.
        if (dotNrm2 >= dotNrm3)
        {
            // Our edges are 1 and 2.
            scratchVec3c = GetNormal(prism->eNrm1Idx);
            scratchVec3d = GetNormal(prism->eNrm2Idx);

            t1 = scratchVec3c.Dot(scratchVec3d);
            if (dotNrm2 >= t1 * dotNrm1)
                dst.classification = Vertex1;
        }
        else
        {
            // Our edges are 3 and 1.
            scratchVec3c = GetNormal(prism->eNrm3Idx);
            scratchVec3d = GetNormal(prism->eNrm1Idx);

            t1 = scratchVec3c.Dot(scratchVec3d);
            if (dotNrm3 >= t1 * dotNrm1)
                dst.classification = Vertex3;
        }

        if (dst.classification == None)
        {
            if (dotNrm1 > dotFaceNrm)
                return false;

            dst.classification = Edge1;
            distance = sqRadius - dotNrm1 * dotNrm1;
        }
    }
    else if (dotNrm2 >= dotNrm1 && dotNrm2 >= dotNrm3 && dotNrm2 >= 0.f)
    {
        // dotNrm2 is the maximum. Pick other edge.
        if (dotNrm1 >= dotNrm3)
        {
            // Our edges are 1 and 2.
            scratchVec3c = GetNormal(prism->eNrm1Idx);
            scratchVec3d = GetNormal(prism->eNrm2Idx);

            t1 = scratchVec3c.Dot(scratchVec3d);
            if (dotNrm1 >= t1 * dotNrm2)
                dst.classification = Vertex1;
        }
        else
        {
            // Our edges are 2 and 3.
            scratchVec3c = GetNormal(prism->eNrm2Idx);
            scratchVec3d = GetNormal(prism->eNrm3Idx);

            t1 = scratchVec3c.Dot(scratchVec3d);
            if (dotNrm3 >= t1 * dotNrm2)
                dst.classification = Vertex2;
        }

        if (dst.classification == None)
        {
            if (dotNrm2 > dotFaceNrm)
                return false;

            dst.classification = Edge2;
            distance = sqRadius - dotNrm2 * dotNrm2;
        }
    }
    else if (dotNrm3 >= dotNrm1 && dotNrm3 >= dotNrm1 && dotNrm3 >= 0.f)
    {
        // dotNrm3 is the maximum. Pick other edge.
        if (dotNrm2 >= dotNrm1)
        {
            // Our edges are 2 and 3.
            scratchVec3c = GetNormal(prism->eNrm2Idx);
            scratchVec3d = GetNormal(prism->eNrm3Idx);

            t1 = scratchVec3c.Dot(scratchVec3d);
            if (dotNrm2 >= t1 * dotNrm3)
                dst.classification = Vertex2;
        }
        else
        {
            // Our edges are 3 and 1.
            scratchVec3c = GetNormal(prism->eNrm2Idx);
            scratchVec3d = GetNormal(prism->eNrm1Idx);

            t1 = scratchVec3c.Dot(scratchVec3d);
            if (dotNrm1 >= t1 * dotNrm3)
                dst.classification = Vertex3;
        }

        if (dst.classification == None)
        {
            if (dotNrm3 > dotFaceNrm)
                return false;

            dst.classification = Edge3;
            distance = sqRadius - dotNrm3 * dotNrm3;
        }
    }
    else 
    {
        // All three of our dot products are <= 0.0. We're on the prism.
        dst.classification = Plane;
    }

    // At this point, everything should be classified.

    if (dst.classification == Vertex1
        || dst.classification == Vertex2
        || dst.classification == Vertex3)
    {
        float squareDistance = 0;
        if (dst.classification == Vertex1)
            squareDistance = CalculateVertexSquareDistance(t1, dotNrm2, dotNrm1, scratchVec3c, scratchVec3d);
        else if (dst.classification == Vertex2)
            squareDistance = CalculateVertexSquareDistance(t1, dotNrm3, dotNrm2, scratchVec3c, scratchVec3d);
        else if (dst.classification == Vertex3)
            squareDistance = CalculateVertexSquareDistance(t1, dotNrm1, dotNrm3, scratchVec3c, scratchVec3d);

        if (squareDistance > (dotFaceNrm * dotFaceNrm) || squareDistance >= sqRadius)
            return false;

        distance = sqRadius - squareDistance;
 
        dst.distance = sqrtf(distance) - dotFaceNrm;
    }
    else if (dst.classification == Edge1
        || dst.classification == Edge2
        || dst.classification == Edge3)
    {
        // dst.distance is already set at this point.
        dst.distance = sqrtf(distance) - dotFaceNrm;
    }
    else if (dst.classification == Plane)
    {
        dst.distance = radius - dotFaceNrm;
    }

    // fx32 maxDist = FX_Mul(_kcl->prismThickness, thickness);
    float maxDist = _kcl->prismThickness * thickness;
    if (dst.distance < 0 || dst.distance > maxDist)
        return false;

    return true;
}

bool KCollisionServer::CheckArrow(CollisionResult* result, u32 maxCollisions, const Vector3& origin, const Vector3& direction, float length)
{
    if (!_kcl)
        return false;

    Vector3 arrow = direction * length;
    Vector3 blkArrowDir = direction;
    Vector3 blkOrigin = origin - Vector3(_kcl->areaMinPosx, _kcl->areaMinPosy, _kcl->areaMinPosz);

    float arrowLength = length;
    float bestLen = arrowLength;

    if (!IsInsideMinMaxInLocalSpace(blkOrigin) && blkArrowDir.x != 0.f)
    {
        float bounds = blkArrowDir.x > 0.f ? 0.f : ~_kcl->areaXWidthMask;
        float length = (bounds - blkOrigin.x) / blkArrowDir.x;
        if (length >= 0.f && length <= arrowLength)
        {
            // Clip ray origin to intersection point.
            blkOrigin += blkArrowDir * length;
            arrowLength -= length;
        }
    }

    if (!IsInsideMinMaxInLocalSpace(blkOrigin) && blkArrowDir.y != 0.f)
    {
        float bounds = blkArrowDir.y > 0.f ? 0.f : ~_kcl->areaYWidthMask;
        float length = (bounds - blkOrigin.y) / blkArrowDir.y;
        if (length >= 0.f && length <= arrowLength)
        {
            // Clip ray origin to intersection point.
            blkOrigin += blkArrowDir * length;
            arrowLength -= length;
        }
    }

    if (!IsInsideMinMaxInLocalSpace(blkOrigin) && blkArrowDir.z != 0.f)
    {
        float bounds = blkArrowDir.z > 0.f ? 0.f : ~_kcl->areaZWidthMask;
        float length = (bounds - blkOrigin.z) / blkArrowDir.z;
        if (length >= 0.f && length <= arrowLength)
        {
            // Clip ray origin to intersection point.
            blkOrigin += blkArrowDir * length;
            arrowLength -= length;
        }
    }

    if (!IsInsideMinMaxInLocalSpace(blkOrigin))
        return false;

    int dstPrismCount = 0;

    while (true)
    {
        if (arrowLength < 0.f)
            break;

        if (!IsInsideMinMaxInLocalSpace(blkOrigin))
            break;

        struct {int x,y,z;} difference;
        difference.x = blkOrigin.x;
        difference.y = blkOrigin.y;
        difference.z = blkOrigin.z;

        // Get the first node from the tree.
        u32 shift = _kcl->blockWidthShift;
        u32* octreeRoot = _blockData;
        u32 curNode = *(octreeRoot +
            (((difference.z >> shift) << _kcl->areaXYBlocksShift)
                | ((difference.y >> shift) << _kcl->areaXBlocksShift)
                | (difference.x >> shift)));

        // Loop through the tree to find the corresponding leaf.
        while (!(curNode >> 31))
        {
            octreeRoot = (u32*)(((u8*)octreeRoot) + curNode);
            shift--;
            curNode = *(octreeRoot +
                (((difference.x >> shift) & 1)
                    | (4 * ((difference.z >> shift) & 1))
                    | (2 * ((difference.y >> shift) & 1))));
        }

        u16* indices = (uint16_t*)((uint8_t*)octreeRoot + (curNode & 0x7FFFFFFF));

        while (true)
        {
            indices++;

            if (*indices == 0)
                break;

            // Get a pointer to the plane information.
            kcol_prism_data_t* prism = &_prisms[*indices];

            KC_PrismHit prismHitScratch;

            if (!KCHitArrow(prismHitScratch, prism, origin, arrow))
                continue;

            result->prisms[dstPrismCount] = prism;
            result->distances[dstPrismCount] = prismHitScratch.distance;
            dstPrismCount++;
            result->length = dstPrismCount;

            if (dstPrismCount >= maxCollisions)
            {
                // We've filled in all the prisms. We're done.
                return true;
            }
        }

        // Continue our search along the octree to the next block.
        u32 mask = (1 << shift) - 1;

        float minLength = INFINITY;

        if (fabsf(blkArrowDir.x) >= 0.001f)
        {
            int bounds;
            if (blkArrowDir.x >= 0)
                bounds = ((mask + 1) - (difference.x & mask)) + 1;
            else
                bounds = -(difference.x & mask) - 1;

            float length = bounds / blkArrowDir.x;
            if (length < minLength)
                minLength = length;
        }

        if (fabsf(blkArrowDir.y) >= 0.001f)
        {
            int bounds;
            if (blkArrowDir.y >= 0)
                bounds = ((mask + 1) - (difference.y & mask)) + 1;
            else
                bounds = -(difference.y & mask) - 1;

            float length = bounds / blkArrowDir.y;
            if (length < minLength)
                minLength = length;
        }

        if (fabsf(blkArrowDir.z) >= 0.001f)
        {
            int bounds;
            if (blkArrowDir.z >= 0)
                bounds = ((mask + 1) - (difference.z & mask)) + 1;
            else
                bounds = -(difference.z & mask) - 1;

            float length = bounds / blkArrowDir.z;
            if (length < minLength)
                minLength = length;
        }

        blkOrigin += blkArrowDir * minLength;
        arrowLength -= minLength;
    }

    return dstPrismCount > 0;
}

bool KCollisionServer::CheckSphere(CollisionResult* result, u32 maxCollisions, const Vector3& position, float radius, float scale)
{
    if (!_kcl)
        return false;

    Vector3 dFloat = position - Vector3(_kcl->areaMinPosx, _kcl->areaMinPosy, _kcl->areaMinPosz);
    struct {int x,y,z;} difference;
    difference.x = dFloat.x;
    difference.y = dFloat.y;
    difference.z = dFloat.z;

    if (!IsInsideMinMaxInLocalSpace(dFloat))
        return false;

    // Get the first node from the tree.
    u32 shift = _kcl->blockWidthShift + 12;
    u32* octreeRoot = _blockData;
    u32 curNode = *(octreeRoot +
        (((difference.z >> shift) << _kcl->areaXYBlocksShift)
            | ((difference.y >> shift) << _kcl->areaXBlocksShift)
            | (difference.x >> shift)));

    // Loop through the tree to find the corresponding leaf.
    while (!(curNode >> 31))
    {
        octreeRoot = (u32*)(((u8*)octreeRoot) + curNode);
        shift--;
        curNode = *(octreeRoot +
            (((difference.x >> shift) & 1)
                | (4 * ((difference.z >> shift) & 1))
                | (2 * ((difference.y >> shift) & 1))));
    }

    u16* indices = (uint16_t*)((uint8_t*)octreeRoot + (curNode & 0x7FFFFFFF));

    // Skip the first index.
    indices++;

    u32 dstPrismCount = 0;

    // Loop through all indices until a null index is found.
    while (*indices)
    {
        kcol_prism_data_t* prism = &_prisms[*indices++];

        KC_PrismHit hit;

        if (!KCHitSphere(hit, prism, position, radius, scale))
            continue;

        result->prisms[dstPrismCount] = prism;
        result->distances[dstPrismCount] = hit.distance;
        result->classifications[dstPrismCount] = hit.classification;
        dstPrismCount++;
        result->length = dstPrismCount;

        if (dstPrismCount >= maxCollisions)
        {
            // We've filled in all the prisms. We're done.
            return true;
        }
    }

    return dstPrismCount > 0;
}
