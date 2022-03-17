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
			void UpdateProgress(float progress) { progression = progress; }

		protected:
			void InitWorld();

			float progression = 0.0f;

			GameWorld* world;
			Renderer* renderer;
		};
	}
}



