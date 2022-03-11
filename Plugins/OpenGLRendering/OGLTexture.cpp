/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "OGLTexture.h"
#include "OGLRendererAPI.h"

#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace NCL::Rendering;

OGLTexture::OGLTexture()
{
	glGenTextures(1, &texID);
}

OGLTexture::OGLTexture(GLuint texToOwn) {
	texID = texToOwn;
}

OGLTexture::~OGLTexture()
{
	glDeleteTextures(1, &texID);
}

void OGLTexture::Bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	switch (type)
	{
	default:
		break;
	case TextureType::TEXTURE_2D:
		glBindTexture(GL_TEXTURE_2D, texID);
		break;
	case TextureType::TEXTURE_CUBEMAP:
		glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
		break;
	}
}

TextureBase* OGLTexture::RGBATextureFromData(char* data, int width, int height, int channels) {
	OGLTexture* tex = new OGLTexture();
	tex->width = width;
	tex->height = height;
	tex->type = TextureType::TEXTURE_2D;

	int dataSize = width * height * channels; //This always assumes data is 1 byte per channel

	int sourceType = GL_RGB;

	switch (channels) {
	case 1: sourceType = GL_RED; break;

	case 2: sourceType = GL_RG; break;
	case 3: sourceType = GL_RGB; break;
	case 4: sourceType = GL_RGBA; break;
		//default:
	}

	glBindTexture(GL_TEXTURE_2D, tex->texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, sourceType, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

TextureBase* OGLTexture::RGBATextureFromFilename(const std::string& name) {
	
	char* texData = nullptr;
	int width = 0;
	int height = 0;
	int channels = 0;
	int flags = 0;
	TextureLoader::LoadTexture(name, texData, width, height, channels, flags);

	TextureBase* glTex = RGBATextureFromData(texData, width, height, channels);
	glTex->type = TextureType::TEXTURE_2D;
	free(texData);

	return glTex;
}

TextureBase* OGLTexture::RGBATextureCubemapFromFilename(const std::string& side1, const std::string& side2, const std::string& side3,
	const std::string& side4, const std::string& side5, const std::string& side6) {
	GLuint tex;
	std::string filenames[6] = {
		side1,
		side2,
		side3,
		side4,
		side5,
		side6,
	};

	int width[6] = { 0 };
	int height[6] = { 0 };
	int channels[6] = { 0 };
	int flags[6] = { 0 };

	std::vector<char*> texData(6, nullptr);

	for (int i = 0; i < 6; ++i) {
		TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return NULL;
		}
	}
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	OGLTexture* cubeMap = new OGLTexture(tex);
	cubeMap->type = TextureType::TEXTURE_CUBEMAP;
	return cubeMap ;
}

TextureBase* OGLTexture::RGBATextureEmpty(int width, int height) {
	GLuint mask;
	glGenTextures(1, &mask);
	glBindTexture(GL_TEXTURE_2D, mask);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	OGLTexture* tex = new OGLTexture(mask);
	tex->type = TextureType::TEXTURE_2D;
	tex->height = height;
	tex->width = width;
	return tex;
}