#include "3ds.h"
#include "citro3d.h"

class Graphics
{
public:
    // This is the maximum amount of vertices that can be loaded at the same time. Raise if needed.
    static constexpr size_t MAX_VERTEX_COUNT = 10000;

    struct GPUVertex // Defined with AttrInfo_AddLoader, BufInfo_Add, and the geometry shader
    {
        struct
        {
            float x, y, z;
        } position; // Shader input v0
        struct
        {
            float u,v;
        } texcoord; // Shader input v1
        struct
        {
            float x, y, z;
        } normal; // Shader input v2
        struct
        {
            float r, g, b, a;
        } color; // Shader input v3
    };
    
    static void SceneInit(void);

private:
    static DVLB_s* vshader_dvlb;
    static shaderProgram_s program;
    static int uLoc_projection, uLoc_modelView;
    static C3D_Mtx projection;
    static GPUVertex* vbo_data;
    static void InitVBO(void);
};
