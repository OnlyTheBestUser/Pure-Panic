#ifdef _ORBIS
#include "PS4Input.h"
#include "pad.h"
#include <sys\_defines\_sce_ok.h>
#include <iostream>
#include <user_service\user_service_api.h>
using namespace NCL::PS4;

PS4Input* ps4input = nullptr;

PS4Input* PS4Input::GetInstance() {
	if (!ps4input) {
		ps4input = new PS4Input();
	}
	return ps4input;
}

PS4Input::PS4Input()	{
	SceUserServiceInitializeParams params;
	memset((void*)&params, 0, sizeof(params));
	params.priority = SCE_KERNEL_PRIO_FIFO_DEFAULT;
	if (sceUserServiceInitialize(&params) != SCE_OK) {
		std::cout << "PS4Input: Failed to initialise User Service!" << std::endl;
	};

	scePadInit();
	InitController();
}

PS4Input::~PS4Input()	{
	scePadClose(padHandle);

	sceUserServiceTerminate();
}

void PS4Input::InitController() {
	//Get the currently logged in player
	SceUserServiceUserId userId;
	int ret = sceUserServiceGetInitialUser(&userId);

	if (ret == SCE_OK) {
		padHandle = scePadOpen(userId, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);

		//Can get connectivity / deadzone info from this structure.
		int ret = scePadGetControllerInformation(padHandle, &padInfo);
		if (ret == SCE_OK) {
			//can get deadzone information etc through here
		}
	}
}

void PS4Input::Poll() {
	ScePadData data;

	int ret = scePadReadState(padHandle, &data);

	if (ret == SCE_OK) {
		if (data.connected) {
			//// Left Stick
			//axis[0].x = (((data.leftStick.x / 255.0f) * 2) - 1.0f); 
			//axis[0].y = (((data.leftStick.y / 255.0f) * 2) - 1.0f);

			//// Right Stick
			//axis[1].x = (((data.rightStick.x / 255.0f) * 2) - 1.0f);
			//axis[1].y = (((data.rightStick.y / 255.0f) * 2) - 1.0f);

			//// DPAD (Should probably be moved from axis as they're actual buttons not used for movement or looking in our game)
			//axis[2].x  = 0.0f;
			//axis[2].x += ((data.buttons & SCE_PAD_BUTTON_RIGHT) ? 1.0f : 0.0f);
			//axis[2].x -= ((data.buttons & SCE_PAD_BUTTON_LEFT)  ? 1.0f : 0.0f);

			//axis[2].y  = 0.0f;
			//axis[2].y += ((data.buttons & SCE_PAD_BUTTON_UP)    ? 1.0f : 0.0f);
			//axis[2].y -= ((data.buttons & SCE_PAD_BUTTON_DOWN)  ? 1.0f : 0.0f);


			//buttons[0] = ((data.buttons & SCE_PAD_BUTTON_TRIANGLE) ? 1.0f : 0.0f);
			//buttons[1] = ((data.buttons & SCE_PAD_BUTTON_CIRCLE)   ? 1.0f : 0.0f);
			//buttons[2] = ((data.buttons & SCE_PAD_BUTTON_CROSS)    ? 1.0f : 0.0f);
			//buttons[3] = ((data.buttons & SCE_PAD_BUTTON_SQUARE)   ? 1.0f : 0.0f);

			//buttons[4] = data.analogButtons.l2 / 255.0f;
			//buttons[5] = data.analogButtons.r2 / 255.0f;

			//buttons[6] = ((data.buttons & SCE_PAD_BUTTON_L1) ? 1.0f : 0.0f);
			//buttons[7] = ((data.buttons & SCE_PAD_BUTTON_R1) ? 1.0f : 0.0f);

			//buttons[8] = ((data.buttons & SCE_PAD_BUTTON_L3) ? 1.0f : 0.0f);
			//buttons[9] = ((data.buttons & SCE_PAD_BUTTON_R3) ? 1.0f : 0.0f);

			// Left Stick
			float x = (((data.leftStick.x / 255.0f) * 2) - 1.0f);
			float y = (((data.leftStick.y / 255.0f) * 2) - 1.0f);
			axis[0].x = abs(x) > 0.1f ? x : 0.0f;
			axis[0].y = abs(y) > 0.1f ? y : 0.0f;

			// Right Stick
			x = (((data.rightStick.x / 255.0f) * 2) - 1.0f);
			y = (((data.rightStick.y / 255.0f) * 2) - 1.0f);
			axis[1].x = abs(x) > 0.1f ? x * 5.0f : 0.0f;
			axis[1].y = abs(y) > 0.1f ? y * 3.0f : 0.0f;

			buttons[JUMP]			= ((data.buttons & SCE_PAD_BUTTON_CROSS) ? 1.0f : 0.0f);
			buttons[DESCEND]		= ((data.buttons & SCE_PAD_BUTTON_CIRCLE) ? 1.0f : 0.0f);
			buttons[LOCK]			= ((data.buttons & SCE_PAD_BUTTON_TRIANGLE) ? 1.0f : 0.0f);
			buttons[TOGGLE_PAUSE]	= ((data.buttons & SCE_PAD_BUTTON_START) ? 1.0f : 0.0f);
			buttons[TOGGLE_DEBUG]	= ((data.buttons & SCE_PAD_BUTTON_SQUARE) ? 1.0f : 0.0f);
			buttons[FIRE]			= ((data.buttons & SCE_PAD_BUTTON_R1) ? 1.0f : 0.0f);
			buttons[QUIT] = ((data.buttons & SCE_PAD_BUTTON_OPTIONS) ? 1.0f : 0.0f);

			//buttons[FORWARD]	= ((data.leftStick.y / 255.0f) * 2) - 1.0f;
			//buttons[BACK]		= ((data.leftStick.y / 255.0f) * 2) - 1.0f;
			//buttons[LEFT]		= ((((data.leftStick.x / 255.0f) * 2) - 1.0f) > 0.1f) ? 0.0f : 1.0f;
			//buttons[RIGHT]		= ((((data.leftStick.x / 255.0f) * 2) - 1.0f) < -0.1f) ? 0.0f : 1.0f;

		}
	}
}
#endif