#pragma once
#include "TutorialGame.h"
#include "../CSC8503Common/StateAI.h"
#include "../CSC8503Common/Gate.h"
#include "../CSC8503Common/SpeedUp.h"

namespace NCL {
	namespace CSC8503 {
		class LevelTwo : public TutorialGame
		{
		public:
			LevelTwo() {
				InitWorld();
			}
			virtual ~LevelTwo() {};

		protected:
			virtual void InitWorld() override;

			StateAI* AddEnemyToWorld(Vector3 position);
			Gate* AddGateToWorld(Vector3 position, Vector3 dimensions);
			SpeedUp* AddSpeedUpToWorld(Vector3 position, float speed = 200.0f, float time = 5.0f);

			void InitFloor();
			void InitWalls();

			vector<GameObject*> powerups;
		};
	}
}

