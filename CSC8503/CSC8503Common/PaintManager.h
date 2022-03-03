#pragma once
namespace NCL {
	namespace CSC8503 {

		class PaintManager
		{
		private:
			PaintManager();
			~PaintManager();
			static PaintManager* inst;
		public:
			static PaintManager* GetInstance();
			void initTextures() {
				// initilieses textures per object
			}
		};

	}
}