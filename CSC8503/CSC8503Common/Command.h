#pragma once
#include "GameActor.h"
#include "PhysicsSystem.h"
#include "InputBase.h"
#include "../GameTech/TutorialGame.h"

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

		class FireCommand : public Command {
		public:
			FireCommand(TutorialGame* game) : game(game) {};
			virtual ~FireCommand() {};

			void execute() {
				game->PaintObject();
			}

		protected:
			TutorialGame* game;
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

		class QuitCommand : public Command {
			public:
				QuitCommand(bool* quit, bool* paused) : paused(paused), quit(quit) {};
				virtual ~QuitCommand() {};
				void execute() {
					if (paused)
						*quit = true;
				}
			protected:
				bool* paused;
				bool* quit;
		};
#pragma endregion
	}
}


