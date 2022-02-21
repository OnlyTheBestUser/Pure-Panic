#pragma once
#include "TextureBase.h"
namespace NCL {
	namespace Rendering {
		class FrameBufferBase
		{
		public:
			FrameBufferBase() {};
			~FrameBufferBase() {};

			virtual void AddTexture() = 0;
			virtual TextureBase* GetTexture() const = 0;
		protected:
		};
	}
}

