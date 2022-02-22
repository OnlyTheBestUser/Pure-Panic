#pragma once
#include "Command.h"
#include <vector>
#include "PCInput.h"

namespace NCL {

	namespace CSC8503 {
		class InputHandler {
		public:

			InputHandler() {
#ifdef _ORBIS
				inputBase = new PS4Input();
#endif
#ifndef _ORBIS
				inputBase = new PCInput();
#endif
			}

			~InputHandler() { delete inputBase; delete[] commands; }

			void HandleInput();

			void BindButton(Input action, Command* command) { commands[action] = command; }

		private:

			Command* commands[127];
			InputBase* inputBase;

			//std::vector<Command*> commandsToExecute;
		};
	}
}