#pragma once
#include "TutorialGame.h"

namespace NCL {
	namespace CSC8503 {
		class MainMenu : public TutorialGame
		{
		public:
			MainMenu() {
				InitWorld();
			};
			virtual ~MainMenu() {};

			virtual void UpdateGame(float dt) override;

		protected:
			virtual void InitWorld() override;
		};
	}
}


