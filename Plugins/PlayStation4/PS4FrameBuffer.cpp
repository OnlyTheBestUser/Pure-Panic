#ifdef _ORBIS
#include "PS4FrameBuffer.h"
#include "../../CSC8503/GameTech/RendererBase.h"

using namespace NCL::PS4;

NCL::PS4::PS4FrameBuffer::PS4FrameBuffer() {

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
	this->renderTarget = ps4Tex->target;
}

NCL::Rendering::TextureBase* NCL::PS4::PS4FrameBuffer::GetTexture() const
{
	return PS4Texture::GenTexFromBuffer(renderTarget);
}
#endif

