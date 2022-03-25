#pragma once
#include "GameActor.h"
#include "PhysicsSystem.h"
#include "InputBase.h"
#include "../GameTech/TutorialGame.h"
#include "../GameTech/NetworkedGame.h"
#include "Timer.h"
#include "../GameTech/MainMenu.h"
#include "../../Common/Assets.h"

namespace NCL {
	namespace CSC8503 {
		class Command {
		public:
			virtual ~Command() {};
			virtual void execute() = 0;
		};

		class AxisCommand {
		public:
			virtual ~AxisCommand() {};
			virtual void execute(AXIS* axis) = 0;
		};

#pragma region Actor Commands
		class MoveCommand : public AxisCommand {
		public:
			MoveCommand(GameActor* actor) : actor(actor){};
			virtual ~MoveCommand() {};

			void execute(AXIS* axis) {
				actor->Move(Vector3(axis->x, 0, axis->y));
			}

		protected:
			GameActor* actor;
		};

		class LookCommand : public AxisCommand {
		public:
			LookCommand(GameActor* actor) : actor(actor) {};
			virtual ~LookCommand() {};

			void execute(AXIS* axis) {
				actor->Look(Vector2(axis->x, axis->y));
			}

		protected:
			GameActor* actor;
		};

		class JumpCommand : public Command {
		public:
			JumpCommand(GameActor* actor) : actor(actor) {};
			virtual ~JumpCommand() {};

			void execute() {
				actor->Jump();
			}

		protected:
			GameActor* actor;
		};

		class DescendCommand : public Command {
		public:
			DescendCommand(GameActor* actor) : actor(actor) {};
			virtual ~DescendCommand() {};

			void execute() {
				actor->Descend();
			}

		protected:
			GameActor* actor;
		};

		class PaintFireCommand : public Command {
		public:
			PaintFireCommand(TutorialGame* game) : game(game) {};
			virtual ~PaintFireCommand() {};

			void execute() {
			//	game->PaintObject();
			}

		protected:
			TutorialGame* game;
		};


		class FireCommand : public Command {
		public:	
			FireCommand(GameActor* actor) : actor(actor) {};
			virtual ~FireCommand() {};
			void execute() {
				actor->Fire();
			}

		protected:
			GameActor* actor;
		};
#pragma endregion

#pragma region World Commands
		class ToggleGravityCommand : public Command {
		public:
			ToggleGravityCommand(PhysicsSystem* physics) : physics(physics) {};
			virtual ~ToggleGravityCommand() {};
			void execute() {
				if (physics->GetGravity())
					physics->UseGravity(false);
				else
					physics->UseGravity(true);
			}
		protected:
			PhysicsSystem* physics;
		};

		class ToggleBoolCommand : public Command {
		public:
			ToggleBoolCommand(bool* toggleVar) : toggleVar(toggleVar) {};
			virtual ~ToggleBoolCommand() {};
			void execute() {
				*toggleVar = !(*toggleVar);
			}
		protected:
			bool* toggleVar;
		};

		class StartTimerCommand : public Command {
		public:
			StartTimerCommand(Timer* timer) : timerVar(timer) {};
			virtual ~StartTimerCommand() {};
			void execute() {
				timerVar->StartTimer();
			}
		protected:
			Timer* timerVar;
		};

		class QuitCommand : public Command {
			public:
				QuitCommand(bool* quit, bool* paused, bool* won) : paused(paused), quit(quit), won(won) {};
				virtual ~QuitCommand() {};
				void execute() {
					if (*paused || *won) {
						*quit = true;
#ifndef _ORBIS
						NCL::BGMManager::GetInstance()->PlaySongFade(Assets::AUDIODIR + "menu_music.ogg", 0.1f);
#endif
					}
				}
			protected:
				bool* paused;
				bool* quit;
				bool* won;
		};

		class ResetWorldCommand : public Command {
		public:
			ResetWorldCommand(GameState* s) : state(s) {};
			virtual ~ResetWorldCommand() {};
			void execute() {
				*state = GameState::RESET;
			}
		protected:
			GameState* state;
		};

		class ToggleMouseCommand : public Command {
		public:
			ToggleMouseCommand(bool* mouse) : mouse(mouse) {};
			virtual ~ToggleMouseCommand() {};
			void execute() {
				if (!*mouse) {
					Window::GetWindow()->ShowOSPointer(true);
					Window::GetWindow()->LockMouseToWindow(false);
					*mouse = true;
				}
				else {
					Window::GetWindow()->ShowOSPointer(false);
					Window::GetWindow()->LockMouseToWindow(true);
					*mouse = false;
				}
			}
		protected:
			bool* mouse;
		};
#pragma endregion

#pragma region Multiplayer Commands

		class StartServerCommand : public Command {
		public:
			StartServerCommand(NetworkedGame* game) : game(game) {};
			virtual ~StartServerCommand() {};
			void execute() {
				game->StartAsServer();
			}

		protected:
			NetworkedGame* game;
		};

		class StartClientCommand : public Command {
		public:
			StartClientCommand(NetworkedGame* game) : game(game) {};
			virtual ~StartClientCommand() {};
			void execute() {
				game->StartAsClient(10, 70, 32, 238);
			}

		protected:
			NetworkedGame* game;
		};

		class StartGameCommand : public Command {
		public:
			StartGameCommand(NetworkedGame* game) : game(game) {};
			virtual ~StartGameCommand() {};
			void execute() {
				game->StartLevel();
			}

		protected:
			NetworkedGame* game;
		};

		class ResetGameCommand : public Command {
		public:
			ResetGameCommand(NetworkedGame* game) : game(game) {};
			virtual ~ResetGameCommand() {};
			void execute() {
				game->ServerResetLevel();
			}

		protected:
			NetworkedGame* game;
		};

#pragma endregion

#pragma region Menu Commands

		class MenuEnterCommand : public Command {
		public:
			MenuEnterCommand(MainMenu* menu) : menu(menu) {};
			virtual ~MenuEnterCommand() {};
			void execute() {
				menu->HandleMenuPress();
			}
		protected:
			MainMenu* menu;
		};

		class MenuMoveCommand : public AxisCommand{
		public:
			MenuMoveCommand(MainMenu* menu) : menu(menu) {};
			virtual ~MenuMoveCommand() {};

			void execute(AXIS* axis) {
				menu->HandleMenuMove({axis->x,axis->y});
			}

		protected:
			MainMenu* menu;
		};
#pragma endregion
	}
}


