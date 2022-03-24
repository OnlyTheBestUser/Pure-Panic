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
			static void SetCompletionState(bool state) { singleton->completed = state; }
			static bool GetCompletionState() { return singleton->completed; }
			static void AddProgress(float progress) { singleton->progression += progress; }
			static void SetInstancesToLoad(int instances) { singleton->instancesToLoad += instances; }

		protected:
			static LoadingScreen* singleton;
			int instancesToLoad;
			float progression = 0.0f;
			bool completed;

			GameWorld* world;
			Renderer* renderer;
		};
	}
}