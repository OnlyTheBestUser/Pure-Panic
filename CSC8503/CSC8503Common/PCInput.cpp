#include "PCInput.h"
#include "../../Common/Keyboard.h"
#include "../../Common/Window.h"

using namespace NCL;
using namespace CSC8503;

PCInput::PCInput()
{
}

PCInput::~PCInput()
{
}

void PCInput::Poll()
{
	buttons[JUMP] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SPACE) ? 1.0f : 0.0f;
	buttons[FIRE] = Window::GetMouse()->ButtonDown(MouseButtons::LEFT) ? 1.0f : 0.0f;
	buttons[TOGGLE_GRAV] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::G) ? 1.0f : 0.0f;
	buttons[TOGGLE_DEBUG] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::J) ? 1.0f : 0.0f;
	buttons[FORWARD] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W) ? 1.0f : 0.0f;
	buttons[BACK] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S) ? 1.0f : 0.0f;
	buttons[LEFT] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A) ? 1.0f : 0.0f;
	buttons[RIGHT] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D) ? 1.0f : 0.0f;

}
