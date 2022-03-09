#include "InputHandler.h"
#include "../../Common/Keyboard.h"
#include "../../Common/Window.h"

using namespace NCL;
using namespace CSC8503;

void InputHandler::HandleInput()
{
	inputBase->ResetInput();
	inputBase->Poll();

	for (int i = 0; i < 127; i++) {
		if (inputBase->GetButtonDown(i)) {
			if(commands[i])
				commands[i]->execute();
		}

		if (axisCommands[i]) {
			axisCommands[i]->execute(inputBase->GetAxis(i));
		}
	}
}

