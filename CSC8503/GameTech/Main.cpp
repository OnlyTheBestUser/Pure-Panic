#ifdef _WIN64
#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "TutorialGame.h"
#include "MainMenu.h"
#include "../CSC8503Common/BehaviourAction.h"
#include "../CSC8503Common/BehaviourSequence.h"
#include "../CSC8503Common/BehaviourSelector.h"

#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"

using namespace NCL;
using namespace CSC8503;

class WinGame : public PushdownState {
public:
	WinGame(TutorialGame* g) : g(g) {};

	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		g->UpdateGame(dt);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
			g->ResetGame();
			g->UpdateGame(dt);
			return PushdownResult::Pop;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Reset;
		}

		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		g->SetState(WIN);
	}

protected:
	TutorialGame* g;
};

class PauseGame : public PushdownState {
public:
	PauseGame(TutorialGame* g) : g(g) {};

	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		g->UpdateGame(dt);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
			return PushdownResult::Pop;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Reset;
		}
		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		g->SetState(PAUSE);
	}

protected:
	TutorialGame* g;
};

class Game : public PushdownState {
public:
	Game(TutorialGame* g) : g(g) {};

	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		g->UpdateGame(dt);

		if (g->Win()) {
			*newState = new WinGame(g);
			return PushdownResult::Push;
		}
		

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
			*newState = new PauseGame(g);
			return PushdownResult::Push;
		}

		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		g->SetState(PLAY);
	}

protected:
	TutorialGame* g;
};

class Menu : public PushdownState {
public:
	Menu(MainMenu* m, TutorialGame* g, TutorialGame* f, TutorialGame* h) : m(m), g(g), f(f), h(h) {};

	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		m->UpdateGame(dt);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1)) {
			*newState = new Game(f);
			return PushdownResult::Push;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2)) {
			*newState = new Game(h);
			return PushdownResult::Push;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM3)) {
			*newState = new Game(g);
			return PushdownResult::Push;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Exit;
		}

		return PushdownResult::NoChange;
	}

protected:
	TutorialGame* g;
	TutorialGame* f;
	TutorialGame* h;
	MainMenu* m;
};

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
	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1600, 900);

	//TestPushdownAutomata(w);

	if (!w->HasInitialised()) {
		return -1;
	}	
	srand(time(NULL));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	//TestBehaviourTree();

	TutorialGame* g = new TutorialGame();
	MainMenu* m = new MainMenu();
	PushdownMachine p = new Menu(m, g, g, g);
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	while (w->UpdateWindow()) { //&& !w->GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
		//DisplayPathfinding();
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		if (!p.Update(dt)) {
			return 0;
		}
		//f->UpdateGame(dt);
		//g->UpdateGame(dt);
	}
	Window::DestroyGameWindow();
}
#endif
#ifdef _ORBIS
#include <stdlib.h>
unsigned int sceLibcHeapExtendedAlloc = 1;			/* Switch to dynamic allocation */
size_t       sceLibcHeapSize = 256 * 1024 * 1024;	/* Set up heap area upper limit as 256 MiB */

#include "../../Plugins/PlayStation4/PS4Window.h"
#include "../../Plugins/PlayStation4/Ps4AudioSystem.h"
#include "../../Plugins/PlayStation4/PS4Input.h"
#include "../../Common/Window.h"

#include "TutorialGame.h"

#include <iostream>



using namespace NCL;
using namespace NCL::PS4;

int main(void) {
#ifdef _ORBIS
	Window* w = (PS4Window*)Window::CreateGameWindow("PS4 Example Code", 1920, 1080);
	PS4Input		input = PS4Input();
	Ps4AudioSystem* audioSystem = new Ps4AudioSystem(8);
#endif
#ifdef _WIN64
	Window* w = Window::CreateGameWindow("CSC8503 Game technology!", 1600, 900);
#endif

	//if (!w->HasInitialised()) {
		//return -1;
	//}
	srand(time(NULL));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	TutorialGame* g = new TutorialGame(&input);
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	while (w->UpdateWindow()) { //&& !w->GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();

#ifdef _ORBIS
		input.Poll();
#endif
		

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		g->UpdateGame(dt);
	}
	Window::DestroyGameWindow();

	// PS4 specific code

	/*
	PS4Input		input = PS4Input();

	Ps4AudioSystem* audioSystem = new Ps4AudioSystem(8);

	GameTimer t;

	delete audioSystem;

	return 1;*/
}

#endif
