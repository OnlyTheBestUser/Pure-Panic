#pragma once
#include "../../Common/UniformBuffer.h"
#include "PS4RendererAPI.h"
namespace NCL {
	namespace PS4 {
		class PS4UniformBuffer : public NCL::Rendering::UniformBuffer
		{
		friend class PS4RendererAPI;
		public:
			PS4UniformBuffer(uint32_t size);
			virtual ~PS4UniformBuffer();


			void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

		private:
			void* data;
		};
	}
}

