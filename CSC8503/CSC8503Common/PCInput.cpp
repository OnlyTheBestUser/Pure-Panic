#ifndef _ORBIS
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
	axis[0].x = 0.0f;
	axis[0].x += Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D) ? 1.0f : 0.0f;
	axis[0].x -= Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A) ? 1.0f : 0.0f;

	axis[0].y = 0.0f;
	axis[0].y -= Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W) ? 1.0f : 0.0f;
	axis[0].y += Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S) ? 1.0f : 0.0f;

	// MOUSE
	axis[1].x = Window::GetMouse()->GetRelativePosition().x;
	axis[1].y = Window::GetMouse()->GetRelativePosition().y;

	buttons[JUMP]			= Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SPACE) ? 1.0f : 0.0f;
	buttons[FIRE]			= Window::GetMouse()->ButtonDown(MouseButtons::LEFT) ? 1.0f : 0.0f;
	buttons[LOCK]			= Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L) ? 1.0f : 0.0f;
	buttons[QUIT]			= Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::ESCAPE) ? 1.0f : 0.0f;
	buttons[DESCEND]		= Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SHIFT) ? 1.0f : 0.0f;
	buttons[TOGGLE_GRAV]	= Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::G) ? 1.0f : 0.0f;
	buttons[TOGGLE_DEBUG]	= Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::J) ? 1.0f : 0.0f;
	buttons[TOGGLE_PAUSE]	= Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::P) ? 1.0f : 0.0f;
	buttons[TOGGLE_MOUSE]	= Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::Q) ? 1.0f : 0.0f;
	buttons[RESET_WORLD] 	= Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::F1) ? 1.0f : 0.0f;
	buttons[QUIT]			= Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::ESCAPE) ? 1.0f : 0.0f;
	buttons[START_TIMER]	= Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::T) ? 1.0f : 0.0f;
}
#endif