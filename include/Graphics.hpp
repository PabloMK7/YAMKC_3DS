#pragma once
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
            float u, v;
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
    
    using GPUFace = std::tuple<Graphics::GPUVertex, Graphics::GPUVertex, Graphics::GPUVertex>;

    static void SceneInit(void);
    static void SceneExit(void);

    static void StartUIDraw(C3D_RenderTarget* target);

    class VertexArray
    {
    public:
        static VertexArray* Create();
        static void Dispose(VertexArray*);

        void AddVertex(const Graphics::GPUVertex& vertex);
        Graphics::GPUVertex& GetVertex(u32 index);
        void Complete();
        u32 Size();

        void Draw(GPU_Primitive_t type);
    private:
        VertexArray(u32 startInd) : startIndex(startInd), size(0) {};
        ~VertexArray() {};

        u32 startIndex;
        u32 size;

        static bool lockCreate;
        static u32 currIndex;
    };
    
    static C3D_Mtx* GetProjectionMtx() {return MtxStack_Cur(&projStack);}
    static C3D_Mtx* GetModelViewMtx() {return MtxStack_Cur(&modelStack);}

    static C3D_Mtx* PushProjectionMtx() {return MtxStack_Push(&projStack);}
    static C3D_Mtx* PushModelViewMtx() {return MtxStack_Push(&modelStack);}
    static C3D_Mtx* PopProjectionMtx() {return MtxStack_Pop(&projStack);}
    static C3D_Mtx* PopModelViewMtx() {return MtxStack_Pop(&modelStack);}

    static void UpdateProjectionMtx() {MtxStack_Update(&projStack);}
    static void UpdateModelViewMtx() {MtxStack_Update(&modelStack);}

friend class VertexArray;
private:
    static void InitVBO(void);
    static void ExitVBO(void);
    
    // This is the maximum amount of vertices that can be loaded at the same time. Raise if needed.
    static constexpr size_t MAX_VERTEX_COUNT = 30000;

    static DVLB_s* vshader_dvlb;
    static shaderProgram_s program;
    static int uLoc_projection, uLoc_modelView;
    static C3D_MtxStack projStack;
    static C3D_MtxStack modelStack;
    static GPUVertex* vbo_data;
};
