#include "Texture.hpp"
#include "Plane.hpp"
#include "citro3d.h"

class BottomMap
{
public:
    BottomMap();
    ~BottomMap();
    void Draw(const Vector3& kartPos, const Angle& rotation);
private:
    float posMinX, posMaxZ, posMaxX, posMinZ;
    Plane backPlane;
    Plane mapPlane;
    Plane charaPlane;
};
