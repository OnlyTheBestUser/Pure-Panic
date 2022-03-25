#ifdef _ORBIS
#include "PS4Texture.h"
#include <fstream>
#include <gnf.h>

#include <.\graphics\api_gnm\toolkit\allocators.h>
#include <.\graphics\api_gnm\toolkit\stack_allocator.h>

#include "../../Common/RendererAPI.h"
#include "PS4RendererAPI.h"

using namespace sce;
using namespace NCL::PS4;
PS4Texture::PS4Texture()
{
}


PS4Texture::~PS4Texture()
{
}

void PS4Texture::Bind(int slot) const {
	Gnm::Sampler trilinearSampler;
	trilinearSampler.init();
	trilinearSampler.setMipFilterMode(Gnm::kMipFilterModeLinear);

	((PS4RendererAPI*)RendererAPI::GetInstance())->currentGFXContext->setTextures(Gnm::kShaderStagePs, slot, 1, &GetAPITexture());
	((PS4RendererAPI*)RendererAPI::GetInstance())->currentGFXContext->setSamplers(Gnm::kShaderStagePs, slot, 1, &trilinearSampler);
}

/*

Generating textures like this WILL cause memory leaks. This is due to the stack allocator
used in the PS4 libraries. A dynamic allocator will fix this, but was a low priority due to
the other issues we encountered in PS4 development.
*/

PS4Texture* NCL::PS4::PS4Texture::EmptyTex(int width, int height)
{
	PS4Texture* tex = new PS4Texture();


	PS4::PS4ScreenBuffer* buffer = ((PS4RendererAPI*)RendererAPI::GetInstance())->GenerateScreenBuffer(width, height, true, false, false);
	tex->target = buffer->colourTarget;

	tex->apiTexture.initFromRenderTarget(&buffer->colourTarget, false);
	tex->width = tex->apiTexture.getWidth();
	tex->height = tex->apiTexture.getHeight();

	tex->apiTexture.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);

	return tex;
}

void PS4Texture::ResetTexture() {
	PS4Texture* newTex = EmptyTex(this->width, this->height);
	this->apiTexture = newTex->apiTexture;
	this->target = newTex->target;
}

PS4Texture* PS4Texture::LoadTextureFromFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);

	if (!file) {
		return NULL;
	}

	Gnf::Header header;
	file.read((char*)&header, sizeof(header));

	if (header.m_magicNumber != sce::Gnf::kMagic) { 
		return NULL; //This wasn't actually a gnf file!!!
	}

	char* rawContents = new char[header.m_contentsSize];
	file.read((char*)rawContents, header.m_contentsSize);


	Gnf::Contents* contentsDesc = (Gnf::Contents*)rawContents;

	Gnm::SizeAlign dataParams	= getTexturePixelsSize(contentsDesc, 0);

	void *pixelsAddr = garlicAllocator->allocate(dataParams);
	Gnm::registerResource(nullptr, ownerHandle, pixelsAddr, dataParams.m_size, filename.c_str(), Gnm::kResourceTypeTextureBaseAddress, 0);

	file.seekg(getTexturePixelsByteOffset(contentsDesc, 0), ios::cur); //fast forward in the file a bit
	file.read((char*)pixelsAddr, dataParams.m_size);

	PS4Texture* tex = new PS4Texture();
	tex->apiTexture = *patchTextures(contentsDesc, 0, 1, &pixelsAddr);

	tex->width  = tex->apiTexture.getWidth();
	tex->height = tex->apiTexture.getHeight();
	//tex->bpp	= tex->apiTexture.getDepth();

	tex->apiTexture.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);

	file.close();
	delete[] rawContents;
	return tex;
}

PS4Texture* PS4Texture::LoadSkyboxFromFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);

	if (!file) {
		return NULL;
	}

	Gnf::Header header;
	file.read((char*)&header, sizeof(header));

	if (header.m_magicNumber != sce::Gnf::kMagic) {
		return NULL; //This wasn't actually a gnf file!!!
	}

	char* rawContents = new char[header.m_contentsSize];
	file.read((char*)rawContents, header.m_contentsSize);


	Gnf::Contents* contentsDesc = (Gnf::Contents*)rawContents;

	Gnm::SizeAlign dataParams = getTexturePixelsSize(contentsDesc, 0);

	void* pixelsAddr = garlicAllocator->allocate(dataParams);
	Gnm::registerResource(nullptr, ownerHandle, pixelsAddr, dataParams.m_size, filename.c_str(), Gnm::kResourceTypeTextureBaseAddress, 0);

	file.seekg(getTexturePixelsByteOffset(contentsDesc, 0), ios::cur); //fast forward in the file a bit
	file.read((char*)pixelsAddr, dataParams.m_size);

	PS4Texture* tex = new PS4Texture();
	tex->apiTexture = *patchTextures(contentsDesc, 0, 1, &pixelsAddr);
	tex->apiTexture.setTextureType(Gnm::kTextureTypeCubemap);

	tex->width  = tex->apiTexture.getWidth();
	tex->height = tex->apiTexture.getHeight();
	//tex->bpp	= tex->apiTexture.getDepth();

	tex->apiTexture.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);

	file.close();
	delete[] rawContents;
	return tex;
}
#endif