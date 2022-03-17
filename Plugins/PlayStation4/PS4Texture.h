#pragma once
#ifdef _ORBIS
#include "../../Common/TextureBase.h"
#include <string>
#include <gnm\texture.h>
#include "PS4MemoryAware.h"
namespace NCL::PS4 {
	class PS4Texture :
		public Rendering::TextureBase, public PS4MemoryAware
	{
	public:
		friend class PS4RendererAPI;
		friend class PS4FrameBuffer;
		friend class Renderer;

		~PS4Texture();

		virtual void Bind(int slot = 0) const override;

		static PS4Texture* EmptyTex(int width, int height);
		static PS4Texture* GenTexFromBuffer(const sce::Gnm::RenderTarget& fbo);
		static PS4Texture* LoadTextureFromFile(const std::string& filename);
		static PS4Texture* LoadSkyboxFromFile(const std::string& filename);

		const sce::Gnm::Texture&  GetAPITexture() const{
			return apiTexture;
		}

		sce::Gnm::RenderTarget target;

	protected:
		PS4Texture();
		sce::Gnm::Texture apiTexture;

	};
}
#endif