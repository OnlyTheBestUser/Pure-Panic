#include "InputHandler.h"
#include "../../Common/Keyboard.h"
#include "../../Common/Window.h"

using namespace NCL;
using namespace CSC8503;

void InputHandler::HandleInput()
{

	// Loop through keys
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W)) {
		if (buttonW_) {
			buttonW_->execute();
		}
	}

	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A)) {
		if (buttonA_) {
			buttonA_->execute();
		}
	}

	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S)) {
		if (buttonS_) {
			buttonS_->execute();
		}
	}

	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D)) {
		if (buttonD_) {
			buttonD_->execute();
		}
	}
}
