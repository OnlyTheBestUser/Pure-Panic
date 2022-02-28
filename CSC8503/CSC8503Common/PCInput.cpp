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

	//// DPAD (Should probably be moved from axis as they're actual buttons not used for movement or looking in our game)
	//axis[2].x  = 0.0f;
	//axis[2].x += ((data.buttons & SCE_PAD_BUTTON_RIGHT) ? 1.0f : 0.0f);
	//axis[2].x -= ((data.buttons & SCE_PAD_BUTTON_LEFT)  ? 1.0f : 0.0f);

	//axis[2].y  = 0.0f;
	//axis[2].y += ((data.buttons & SCE_PAD_BUTTON_UP)    ? 1.0f : 0.0f);
	//axis[2].y -= ((data.buttons & SCE_PAD_BUTTON_DOWN)  ? 1.0f : 0.0f);

	axis[0].x = 0.0f;
	axis[0].x += Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D) ? 1.0f : 0.0f;
	axis[0].x -= Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A) ? 1.0f : 0.0f;

	axis[0].y = 0.0f;
	axis[0].y -= Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W) ? 1.0f : 0.0f;
	axis[0].y += Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S) ? 1.0f : 0.0f;

	// MOUSE
	//pitch -= (Window::GetMouse()->GetRelativePosition().y);
	//yaw -= (Window::GetMouse()->GetRelativePosition().x);
	axis[1].x = Window::GetMouse()->GetRelativePosition().x;
	axis[1].y = Window::GetMouse()->GetRelativePosition().y;

	buttons[JUMP] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SPACE) ? 1.0f : 0.0f;
	buttons[FIRE] = Window::GetMouse()->ButtonDown(MouseButtons::LEFT) ? 1.0f : 0.0f;
	buttons[TOGGLE_GRAV] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::G) ? 1.0f : 0.0f;
	buttons[TOGGLE_DEBUG] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::J) ? 1.0f : 0.0f;
	buttons[FORWARD] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W) ? 1.0f : 0.0f;
	buttons[BACK] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S) ? 1.0f : 0.0f;
	buttons[LEFT] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A) ? 1.0f : 0.0f;
	buttons[RIGHT] = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D) ? 1.0f : 0.0f;
	buttons[LOCK] = Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L) ? 1.0f : 0.0f;

}
#endif