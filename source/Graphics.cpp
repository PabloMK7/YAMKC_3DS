#include "Graphics.hpp"
#include "Material.hpp"
#include "vshader_shbin.h"
#include "Vector.hpp"
#include <stdlib.h>

DVLB_s* Graphics::vshader_dvlb = nullptr;
shaderProgram_s Graphics::program;
int Graphics::uLoc_projection, Graphics::uLoc_modelView;
Graphics::GPUVertex* Graphics::vbo_data = nullptr;
C3D_MtxStack Graphics::projStack;
C3D_MtxStack Graphics::modelStack;

bool Graphics::VertexArray::lockCreate = false;
u32 Graphics::VertexArray::currIndex = 0;

void Graphics::SceneInit()
{
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    // Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);

	// Get the location of the uniforms
	uLoc_projection   = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	uLoc_modelView    = shaderInstanceGetUniformLocation(program.vertexShader, "modelView");

	// Create the matrix stacks
	MtxStack_Init(&projStack);
	MtxStack_Bind(&projStack, GPU_VERTEX_SHADER, uLoc_projection, 4);
	MtxStack_Init(&modelStack);
	MtxStack_Bind(&modelStack, GPU_VERTEX_SHADER, uLoc_modelView, 4);

    // Initialize the vertex buffer
    InitVBO();

	Material::InitFragmentLighting();
}

void Graphics::SceneExit(void)
{
	// Exit C3D
	C3D_Fini();
	// Free shader program
	shaderProgramFree(&program);
	// Free DVLB
	DVLB_Free(vshader_dvlb);
	// Exit the vertex buffer
	ExitVBO();
}

static const C3D_FVec laPos = {0.f, 1.f, 0.f, 0.f};
static const C3D_FVec laLookAt = {0.f, 0.f, 0.f, 0.f};
static const C3D_FVec laUp = {0.f, 0.f, 1.f, 0.f};

static const float cameraInterOcularDistanceMultiplier = 2.3f;
static const float cameraFocalLength = 19.f;
static const float cameraFov = 55.f;

void Graphics::Start3DDraw(C3D_RenderTarget* target, float iod)
{
	C3D_RenderTargetClear(target, C3D_CLEAR_DEPTH, 0, 0);

    C3D_Mtx* p = Graphics::GetProjectionMtx();
    Mtx_PerspStereoTilt(p, Angle::DegreesToRadians(cameraFov), 400.f / 240.f, 20, 10000, iod * cameraInterOcularDistanceMultiplier, cameraFocalLength, false);
    Graphics::UpdateProjectionMtx();
}

void Graphics::StartUIDraw(C3D_RenderTarget* target)
{
    C3D_RenderTargetClear(target, C3D_CLEAR_DEPTH, 0, 0);

	Mtx_OrthoTilt(Graphics::GetProjectionMtx(), 0.0f, target->screen == GFX_TOP ? 400.0f : 320.f, 240.0f, 0.0f, 1.f, -1.0f, true);
	Mtx_Identity(Graphics::GetModelViewMtx());

    Graphics::UpdateProjectionMtx();
}

void Graphics::InitVBO(void)
{
    // Configure attributes for use with the vertex shader
	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord
	AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 3); // v2=normal
    AttrInfo_AddLoader(attrInfo, 3, GPU_FLOAT, 4); // v3=color

	// Create the VBO (vertex buffer object)
	vbo_data = (GPUVertex*)linearAlloc(sizeof(GPUVertex) * MAX_VERTEX_COUNT);

	// Configure buffers
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, vbo_data, sizeof(GPUVertex), 4, 0x3210); // Last arg is the order of the input attrs in reverse
}

void Graphics::ExitVBO(void)
{
	if (vbo_data) linearFree(vbo_data);
}

Graphics::VertexArray* Graphics::VertexArray::Create()
{
	if (lockCreate)
		return nullptr;
	lockCreate = true;
	return new VertexArray(currIndex);
}

void Graphics::VertexArray::Dispose(VertexArray* array)
{
	if (array)
		delete array;
}

void Graphics::VertexArray::AddVertex(const Graphics::GPUVertex& vertex)
{
	if (!lockCreate)
		return;
	vbo_data[startIndex + size++] = vertex;
}

static Graphics::GPUVertex defaultV;
Graphics::GPUVertex& Graphics::VertexArray::GetVertex(u32 index)
{
	if (index < size)
		return vbo_data[startIndex + index];
	else
		return defaultV;
}

u32 Graphics::VertexArray::Size()
{
	return size;
}

void Graphics::VertexArray::Complete()
{
	lockCreate = false;
	currIndex = startIndex + size;
	if (currIndex > MAX_VERTEX_COUNT)
		abort();
}

void Graphics::VertexArray::Draw(GPU_Primitive_t type)
{
	C3D_DrawArrays(type, startIndex, size);
}