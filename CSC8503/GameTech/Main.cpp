#include <stdlib.h>
unsigned int sceLibcHeapExtendedAlloc = 1;			/* Switch to dynamic allocation */
size_t       sceLibcHeapSize = 256 * 1024 * 1024;	/* Set up heap area upper limit as 256 MiB */

#include "../../Plugins/PlayStation4/PS4Window.h"
#include "../../Plugins/PlayStation4/Ps4AudioSystem.h"
#include "../../Plugins/PlayStation4/PS4Input.h"
#include "../../Common/Window.h"

#include "TutorialGame.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "TutorialGame.h"
#include "MainMenu.h"
#include "../CSC8503Common/BehaviourAction.h"
#include "../CSC8503Common/BehaviourSequence.h"
#include "../CSC8503Common/BehaviourSelector.h"

#include <iostream>

#include "NetworkedGame.h"
#include "TrainingGame.h"
#include "LoadingScreen.h"

using namespace NCL;
using namespace CSC8503;
using namespace NCL;
using namespace NCL::PS4;

int main() {
#ifdef _WIN64
	Window* w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);
#endif
#ifdef _ORBIS
	Window* w = (PS4Window*)Window::CreateGameWindow("PS4 Example Code", 1920, 1080);
#endif

		
	if (!w->HasInitialised()) {
		return -1;
	}	
	srand(time(NULL));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);
	
	LoadingScreen* l = new LoadingScreen();
	MainMenu menu;

	w->GetTimer()->GetTimeDeltaSeconds(); 

	while (w->UpdateWindow()) {

		float dt = w->GetTimer()->GetTimeDeltaSeconds();

		if (dt > 0.1f) {	//Skipping large time delta
			continue;	//must have hit a breakpoint or something to have a 1 second frame time!
		}

		float frameRate = (1.0f / dt);

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt) + " | Gametech frame rate:" + std::to_string(frameRate));

		if (!menu.UpdateGame(dt)) {
			return 0;
		}
	}
	Window::DestroyGameWindow();
}
