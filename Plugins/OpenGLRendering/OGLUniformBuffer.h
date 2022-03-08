#pragma once
#include "../../Common/UniformBuffer.h"

namespace NCL {
	namespace Rendering {
		class OGLUniformBuffer : public UniformBuffer
		{
		public:
			OGLUniformBuffer(uint32_t size, uint32_t binding);
			virtual ~OGLUniformBuffer();

			virtual void BindBuffer(uint32_t binding) override;
			virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		protected:
			uint32_t RendererID = 0;
		};
	}
}

