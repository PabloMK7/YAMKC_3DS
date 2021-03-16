#include "3ds.h"
#include "citro3d.h"
#include "vector"

class Graphics
{
public:
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

    class VertexArray
    {
    public:
        static VertexArray* Create();
        static void Dispose(VertexArray*);

        void AddVertex(const Graphics::GPUVertex& vertex);
        void Complete();

        void Draw(GPU_Primitive_t type);
    private:
        VertexArray(u32 startInd) : startIndex(startInd), size(0) {};
        ~VertexArray() {};

        u32 startIndex;
        u32 size;

        static bool lockCreate;
        static u32 currIndex;
    };
    
friend class VertexArray;
private:
    static void InitVBO(void);
    
    // This is the maximum amount of vertices that can be loaded at the same time. Raise if needed.
    static constexpr size_t MAX_VERTEX_COUNT = 50000;

    static DVLB_s* vshader_dvlb;
    static shaderProgram_s program;
    static int uLoc_projection, uLoc_modelView;
    static C3D_Mtx projection;
    static C3D_Mtx modelview;
    static GPUVertex* vbo_data;
};
