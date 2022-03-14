#pragma once

namespace NCL {
	namespace Rendering {

		enum class TextureType 
		{
			TEXTURE_2D,
			TEXTURE_CUBEMAP,
		};

		class TextureBase
		{
			friend class OGLTexture;
		public:
			virtual ~TextureBase();

			virtual void Bind(int slot = 0) const = 0;

			int GetWidth() { return width; }
			int GetHeight() { return height;  }
			TextureType GetType() { return type; }
		protected:
			TextureBase();

			int width;
			int height;
			TextureType type = TextureType::TEXTURE_2D;

		};
	}
}

