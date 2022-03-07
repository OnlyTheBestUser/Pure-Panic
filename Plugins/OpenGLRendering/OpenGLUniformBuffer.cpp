#include "OpenGLUniformBuffer.h"
#include "OGLShader.h"

using namespace NCL::Rendering;


OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
{
	glCreateBuffers(1, &RendererID);
	glNamedBufferData(RendererID, size, nullptr, GL_DYNAMIC_DRAW); 
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, RendererID);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
	glDeleteBuffers(1, &RendererID);
}


void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	glNamedBufferSubData(RendererID, offset, size, data);
}
