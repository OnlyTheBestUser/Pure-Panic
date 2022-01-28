#pragma once
#include "GameActor.h"

namespace NCL {
	namespace CSC8503 {
		class Command {
		public:
			virtual ~Command() {};
			virtual void execute() = 0;
		};

#pragma region Actor Commands

		class MoveForwardCommand : public Command {
		public:
			MoveForwardCommand(GameActor* actor) : actor(actor) {};
			virtual ~MoveForwardCommand() {};
			void execute() {
				actor->MoveForwards();
			}

		protected:
			GameActor* actor;
		};

		class MoveBackwardCommand : public Command {
		public:
			MoveBackwardCommand(GameActor* actor) : actor(actor) {};
			virtual ~MoveBackwardCommand() {};
			void execute() {
				actor->MoveBackwards();
			}

		protected:
			GameActor* actor;
		};

		class MoveLeftCommand : public Command {
		public:
			MoveLeftCommand(GameActor* actor) : actor(actor) {};
			virtual ~MoveLeftCommand() {};
			void execute() {
				actor->MoveLeft();
			}

		protected:
			GameActor* actor;
		};

		class MoveRightCommand : public Command {
		public:
			MoveRightCommand(GameActor* actor) : actor(actor) {};
			virtual ~MoveRightCommand() {};
			void execute() {
				actor->MoveRight();
			}

		protected:
			GameActor* actor;
		};

#pragma endregion
	}
}


