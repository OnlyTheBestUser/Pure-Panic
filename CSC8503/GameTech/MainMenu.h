#pragma once
#include "TutorialGame.h"

#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"
#include <algorithm>

namespace NCL {
	namespace CSC8503 {
		class InputHandler;

		class LevelState : public PushdownState {
		public:
			LevelState(TutorialGame* level) : level(level) {};

			PushdownState::PushdownResult OnUpdate(float dt, PushdownState** newState) override {
				if (!level)
					return PushdownResult::Pop;

				level->UpdateGame(dt);

				if (level->GetPaused() && level->GetQuit()) {
					level->SetState(GameState::RESET);
					level->UpdateGame(dt);
					return PushdownResult::Pop;
				}

				return PushdownState::PushdownResult::NoChange;
			}
		protected:
			TutorialGame* level;
		};

		class MainMenu : public PushdownState
		{
		public:
			MainMenu();
			~MainMenu();

			bool UpdateGame(float dt);

			void HandleMenuMove(Vector2 moveInput);
			void HandleMenuPress();

			PushdownState::PushdownResult OnUpdate(float dt, PushdownState** newState) override;

		protected:
			void UpdateMenu(float dt);

			InputHandler* inputHandler;
			RendererBase renderer;

			TutorialGame* networkedLevel;
			TutorialGame* trainingLevel;
			PushdownMachine* pushMachine;

			int selectedItem = 0;
			bool pressed = false;
		};
	}
}


