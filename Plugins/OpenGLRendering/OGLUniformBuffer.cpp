#include "OGLUniformBuffer.h"
#include "OGLShader.h"

using namespace NCL::Rendering;


OGLUniformBuffer::OGLUniformBuffer(uint32_t size, uint32_t binding)
{
	glCreateBuffers(1, &RendererID);
	glNamedBufferData(RendererID, size, nullptr, GL_DYNAMIC_DRAW); 
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, RendererID);
}

OGLUniformBuffer::~OGLUniformBuffer()
{
	glDeleteBuffers(1, &RendererID);
}

void OGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	glNamedBufferSubData(RendererID, offset, size, data);
}
