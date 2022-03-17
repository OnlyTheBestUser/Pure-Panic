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
	this->tex = ps4Tex;
	renderTarget.initFromTexture(&ps4Tex->apiTexture, 1);
	renderTarget.setWidth(ps4Tex->apiTexture.getWidth());
	renderTarget.setHeight(ps4Tex->apiTexture.getHeight());
}

NCL::Rendering::TextureBase* NCL::PS4::PS4FrameBuffer::GetTexture() const
{
	return PS4Texture::GenTexFromBuffer(renderTarget);
}
#endif

