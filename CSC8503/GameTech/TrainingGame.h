#pragma once
#include "TutorialGame.h"
namespace NCL {
	namespace CSC8503 {
		class TrainingGame : public TutorialGame
		{
		public:
			TrainingGame();
			virtual ~TrainingGame() {}

			void InitWorld() override;
			void UpdateWinScreen(float dt) override;
		};
	}
}