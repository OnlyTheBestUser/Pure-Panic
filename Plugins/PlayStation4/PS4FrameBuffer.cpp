#ifdef _ORBIS
#include "PS4FrameBuffer.h"
#include "../../CSC8503/GameTech/RendererBase.h"

using namespace NCL::PS4;

NCL::PS4::PS4FrameBuffer::PS4FrameBuffer() {
	PS4RendererAPI* rend = ((PS4RendererAPI*)NCL::Rendering::RendererBase::rendererAPI);
	/*buffer = rend->GenerateScreenBuffer(rend->currentWidth,
		rend->currentHeight,
		true,
		false,
		false);*/
}

NCL::PS4::PS4FrameBuffer::~PS4FrameBuffer() {

}

void NCL::PS4::PS4FrameBuffer::AddTexture(){

}

void NCL::PS4::PS4FrameBuffer::AddTexture(int width, int height) {

}

void NCL::PS4::PS4FrameBuffer::AddTexture(TextureBase* tex) {
	PS4Texture* ps4Tex = static_cast<PS4Texture*>(tex);
	buffer->colourTarget.initFromTexture(&ps4Tex->GetAPITexture(), 1);
}

NCL::Rendering::TextureBase* NCL::PS4::PS4FrameBuffer::GetTexture() const
{
	return PS4Texture::GenTexFromBuffer(buffer->colourTarget);
}

PS4ScreenBuffer* NCL::PS4::PS4FrameBuffer::GetBuffer() const
{
	return buffer;
}
#endif

