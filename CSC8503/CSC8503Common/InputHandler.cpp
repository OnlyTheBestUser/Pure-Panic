#include "InputHandler.h"
#include "../../Common/Keyboard.h"
#include "../../Common/Window.h"

using namespace NCL;
using namespace CSC8503;

void InputHandler::HandleInput()
{

	// Loop through keys
	if (buttonW_) {
		if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W)) {
			buttonW_->execute();
		}
	}

	if (buttonA_) {
		if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A)) {
			buttonA_->execute();
		}
	}

	if (buttonS_) {
		if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S)) {
			buttonS_->execute();
		}
	}

	if (buttonD_) {
		if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D)) {
			buttonD_->execute();
		}
	}

	if (buttonG_) {
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::G)) {
			buttonG_->execute();
		}
	}

	if (buttonJ_) {
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::J)) {
			buttonG_->execute();
		}
	}
}
