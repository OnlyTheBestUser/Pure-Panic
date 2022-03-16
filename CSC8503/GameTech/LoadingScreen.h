#pragma once
#include "../CSC8503Common/GameWorld.h"
#include "Renderer.h"

namespace NCL {
	namespace CSC8503 {
		class LoadingScreen
		{
		public:
			LoadingScreen();
			~LoadingScreen();

			void UpdateGame(float dt);

		protected:
			void InitCamera();
			void InitWorld();

			GameWorld* world;
			Renderer* renderer;
		};
	}
}



