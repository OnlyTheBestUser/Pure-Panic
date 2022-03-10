#pragma once
#include "Command.h"
#include <vector>
#include "PCInput.h"
#include "../../Plugins/PlayStation4/PS4Input.h"

namespace NCL {

	namespace CSC8503 {
		class InputHandler {
		public:

			InputHandler() {
#ifdef _ORBIS
				inputBase = new PS4::PS4Input();
#endif
#ifndef _ORBIS
				inputBase = new PCInput();
#endif
			}

			~InputHandler() { 
				delete inputBase;
				for (auto x : commands) {
					if (x)
						delete x;
				}
			}

			void HandleInput();
			void BindButton(Input action, Command* command) { commands[action] = command; }
			void BindAxis(int axisIndex, AxisCommand* command) { axisCommands[axisIndex] = command; }

			AXIS* GetAxis(int index) {
				return inputBase->GetAxis(index);
			}

		private:

			Command* commands[127] = {nullptr};
			AxisCommand* axisCommands[127] = {nullptr};
			InputBase* inputBase;

			//std::vector<Command*> commandsToExecute;
		};
	}
}