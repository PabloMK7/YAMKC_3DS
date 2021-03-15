#include "Graphics.hpp"
#include "vshader_shbin.h"
void Graphics::SceneInit()
{
    // Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);

	// Get the location of the uniforms
	uLoc_projection   = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	uLoc_modelView    = shaderInstanceGetUniformLocation(program.vertexShader, "modelView");

    // Initialize the vertex buffer array
    InitVBO();
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