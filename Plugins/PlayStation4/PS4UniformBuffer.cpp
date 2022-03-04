#ifdef _ORBIS
#include "PS4UniformBuffer.h"
#include "PS4Shader.h"
using namespace NCL::PS4;

PS4UniformBuffer::PS4UniformBuffer(PS4RendererAPI* rend, uint32_t size) {
	renderer = rend;
	data = (void*)rend->currentGFXContext->allocateFromCommandBuffer(size, Gnm::kEmbeddedDataAlignment4);

	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(data, size);
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
}

PS4UniformBuffer::~PS4UniformBuffer() {

}

void PS4UniformBuffer::SetData(const void* value, uint32_t size, uint32_t offset) {
	memcpy(data, value, size);
}
#endif