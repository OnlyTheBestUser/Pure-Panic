#pragma once
#include "TutorialGame.h"

#include "../CSC8503Common/InputHandler.h"
#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"
#include <algorithm>

namespace NCL {
	namespace CSC8503 {
		class LevelState : public PushdownState {
		public:
			LevelState(TutorialGame* level) : level(level) {};

			PushdownState::PushdownResult OnUpdate(float dt, PushdownState** newState) override {
				if (!level)
					return PushdownResult::Pop;

				if (level->GetQuit()) {
					level->SetQuit(false);
					level->SetState(GameState::RESET);
					return PushdownResult::Pop;
				}

				level->UpdateGame(dt);
				
				return PushdownState::PushdownResult::NoChange;
			}
		protected:
			TutorialGame* level;
		};

		class MainMenu : public PushdownState
		{
		public:
			MainMenu(TutorialGame* start, TutorialGame* training);
			bool UpdateGame(float dt);

			PushdownState::PushdownResult OnUpdate(float dt, PushdownState** newState) override;

		protected:
			InputHandler* inputHandler;
			RendererBase renderer;

			TutorialGame* networkedLevel;
			TutorialGame* trainingLevel;
			PushdownMachine* pushMachine;

			int selectedItem = 0;
		};
	}
}


