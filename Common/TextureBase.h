#pragma once

namespace NCL {
	namespace Rendering {
		class TextureBase
		{
		public:
			virtual ~TextureBase();

			int GetWidth() { return width; }
			int GetHeight() { return height;  }
		protected:
			TextureBase();

			int width;
			int height;
		};
	}
}

