#include "BottomMap.hpp"

static const float BottomMapScale = 512;
static const Color ClearColor(8, 144, 8);

BottomMap::BottomMap() {
    backPlane.GetMaterial().SetCostantColor(0, ClearColor);
    mapPlane.GetMaterial()
        .SetTexture("romfs:/map/map.t3x", false)
        .SetTexCombineMode(Material::TexCombineModeDeafult, nullptr)
        .SetBlendMode(Material::BlendModeOpacity, nullptr);
    charaPlane.GetMaterial()
        .SetTexture("romfs:/map/chara.t3x")
        .SetTexCombineMode(Material::TexCombineModeDeafult, nullptr)
        .SetBlendMode(Material::BlendModeTransparency, nullptr);
    
    backPlane.GetPosition() = Vector3(160.f, 120.f, -0.1f);
    backPlane.GetScale() = Vector3(320.f, 240.f, 0.f);

    mapPlane.GetScale() = Vector3(BottomMapScale, BottomMapScale, 1.f);
 
    charaPlane.GetPosition() = Vector3(160.f, 120.f, 0.1f);
    charaPlane.GetScale() = Vector3(30.f, 30.f, 1.f);

    FILE* uimap = fopen("romfs:/map/UIMapPos.bin", "rb");
    fseek(uimap, 4 * sizeof(float), SEEK_SET);
    float uimapvals[4];
    fread(uimapvals, sizeof(float), 4, uimap);
    fclose(uimap);
    posMinX = uimapvals[0]; posMaxZ = uimapvals[1]; posMaxX = uimapvals[2]; posMinZ = uimapvals[3];
}

BottomMap::~BottomMap() {}

void BottomMap::Draw(const Vector3& kartPos, const Angle& rotation) {
    
    backPlane.Draw();

    float xProg = 1.f - (kartPos.x - posMinX) / (posMaxX - posMinX);
    float zProg = 1.f - (kartPos.z - posMinZ) / (posMaxZ - posMinZ);


    float centerXPos = ((BottomMapScale * xProg) - (BottomMapScale / 2.f));
    float centerYPos = ((BottomMapScale * zProg) - (BottomMapScale / 2.f));
    float zRot = rotation.AsRadians();

    C3D_Mtx* m = Graphics::PushModelViewMtx();
    Mtx_Translate(m, charaPlane.GetPosition().x, charaPlane.GetPosition().y, 0.f, true);
    Mtx_RotateZ(m, zRot, true);
    Mtx_Translate(m, centerXPos, centerYPos, 0.f, true);
    mapPlane.Draw();
    Graphics::PopModelViewMtx();

    charaPlane.Draw();
}

