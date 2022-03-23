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

using namespace NCL;
using namespace CSC8503;
using namespace NCL;
using namespace NCL::PS4;

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead.

This time, we've added some extra functionality to the window class - we can
hide or show the

*/

int main() {
#ifdef _WIN64
	Window* w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);
#endif
#ifdef _ORBIS
	Window* w = (PS4Window*)Window::CreateGameWindow("PS4 Example Code", 1920, 1080);
	Ps4AudioSystem* audioSystem = new Ps4AudioSystem(8);
#endif

		
	if (!w->HasInitialised()) {
		return -1;
	}	
	srand(time(NULL));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	float avgTimeWait = 3.0f;
	float curTimeWait = 3.0f;
	float totalTime = 0.0f;
	int totalFrames = 0;

	//TutorialGame* g = new TutorialGame();
	NetworkedGame* level = new NetworkedGame();
	// MainMenu* m = new MainMenu();
	// PushdownMachine p = new Menu(m, g, h, g);
	MainMenu menu(level, nullptr);
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	float smallestFrameRate = 144.0f;
	while (w->UpdateWindow()) { //&& !w->GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
#if _WIN64
		if (w->GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
			break;
#endif

		//DisplayPathfinding();
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}*/

		float frameRate = (1.0f / dt);
		if (frameRate < smallestFrameRate)
			smallestFrameRate = frameRate;

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt) + " | Gametech frame rate:" + std::to_string(frameRate));

		curTimeWait -= dt;
		totalTime += dt;
		totalFrames++;
		if (curTimeWait < 0.0f) {
			std::cout << "Average Frame Time: " << 1000.0f * (totalTime / totalFrames) << "\n";
			curTimeWait = avgTimeWait;
		}

		if (!menu.UpdateGame(dt)) {
			return 0;
		}

	}
	Window::DestroyGameWindow();
}
