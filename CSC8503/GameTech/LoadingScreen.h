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

			static void UpdateGame(float dt);
			static void AddProgress(float progress) { singleton->progression += progress; }

		protected:
			static LoadingScreen* singleton;
			float progression = 0.0f;

			GameWorld* world;
			Renderer* renderer;
		};
	}
}



