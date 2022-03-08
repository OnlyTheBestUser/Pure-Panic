#pragma once
#include <cstdint>
namespace NCL {
	namespace Rendering {
		class UniformBuffer
		{
		public:
			virtual ~UniformBuffer() {}
			virtual void BindBuffer(uint32_t binding) = 0;
			virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		};
	}
}

