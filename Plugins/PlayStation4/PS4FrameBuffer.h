#ifdef _ORBIS
#pragma once
#include "../../Common/FrameBufferBase.h"
#include "PS4RendererAPI.h"
#include "PS4Texture.h"
namespace NCL {
    namespace PS4 {
        class PS4FrameBuffer : public FrameBufferBase
        {
        friend class PS4RendererAPI;
        friend class PS4Texture;
        public:
            PS4FrameBuffer();
            ~PS4FrameBuffer();

            virtual void AddTexture() override;
            virtual void AddTexture(int width, int height) override;
            virtual void AddTexture(TextureBase* text) override;
            virtual TextureBase* GetTexture() const override;

        protected:
            Gnm::RenderTarget renderTarget;
            PS4Texture* tex;
        };
    }
}
#endif

