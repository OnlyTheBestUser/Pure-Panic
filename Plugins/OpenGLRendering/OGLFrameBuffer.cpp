#include "OGLFrameBuffer.h"
using namespace NCL::Rendering;
#define SHADOWSIZE 4096

OGLFrameBuffer::OGLFrameBuffer() : FrameBufferBase() {
	glGenFramebuffers(1, &frameBuffer);
	tex = -1;
}

OGLFrameBuffer::~OGLFrameBuffer() {
	glDeleteFramebuffers(1, &frameBuffer);
	delete texObj;
}

void OGLFrameBuffer::AddTexture() {
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	texObj = new OGLTexture(tex);
}

TextureBase* OGLFrameBuffer::GetTexture() const{
	return texObj;
}
