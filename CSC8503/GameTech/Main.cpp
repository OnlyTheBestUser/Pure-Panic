#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "TutorialGame.h"
#include "LevelOne.h"
#include "LevelTwo.h"
#include "MainMenu.h"
#include "../CSC8503Common/BehaviourAction.h"
#include "../CSC8503Common/BehaviourSequence.h"
#include "../CSC8503Common/BehaviourSelector.h"

#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"

using namespace NCL;
using namespace CSC8503;

void TestStateMachine() {
	StateMachine* testMachine = new StateMachine();
	int data = 0;

	State* A = new State([&](float dt)->void {
			std::cout << "I'm state A!\n";
			data++;
		}
	);

	State* B = new State([&](float dt)->void {
			std::cout << "I'm state B!\n";
			data--;
		}
	);

	StateTransition* stateAB = new StateTransition(A, B, [&](void)->bool {
			return data > 10;
		}
	);

	StateTransition* stateBA = new StateTransition(B, A, [&](void)->bool {
			return data < 0;
		}
	);

	testMachine->AddState(A);
	testMachine->AddState(B);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);

	for (int i = 0; i < 100; ++i) {
		testMachine->Update(1.0f);
	}
}

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

class PauseScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U))
			return PushdownResult::Pop;
		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		std::cout << "Press U to unpause game!\n";
	}
};

class GameScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		pauseReminder -= dt;
		if (pauseReminder < 0) {
			std::cout << "Coins mined: " << coinsMined << "\n";
			std::cout << "Press P to pause game, or F1 top return to main menu!\n";
			pauseReminder += 1.0f;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::P)) {
			*newState = new PauseScreen();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1)) {
			std::cout << "Returning to main menu!\n";
			return PushdownResult::Pop;
		}
		if (rand() % 7 == 0) {
			coinsMined++;
		}
		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		std::cout << "Preparing to mine coins!\n";
	}

protected:
	int coinsMined = 0;
	float pauseReminder = 1;
};

class IntroScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			*newState = new GameScreen();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		std::cout << "Welcome to a really awesome game!\nPress Space to Begin or Escape to quite!\n";
	}
};

void TestPushdownAutomata(Window* w) {
	PushdownMachine machine(new IntroScreen());
	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (!machine.Update(dt)) {
			return;
		}
	}
}

vector<Vector3> testNodes;
void TestPathfinding() {
	NavigationGrid grid("TestGrid1.txt");

	NavigationPath outPath;

	Vector3 startPos(20, 0, 20);
	Vector3 endPos(320, 0, 320);

	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos + Vector3(10, 0, 10));
	}
}

void DisplayPathfinding() {
	for (int i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a + Vector3(0, 5, 0), b + Vector3(0, 5, 0), Vector4(0, 1, 0, 1));
	}
}

void TestBehaviourTree() {
	float behaviourTimer;
	float distanceToTarget;
	BehaviourAction* findKey = new BehaviourAction("Find Key", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for a key!\n";
			behaviourTimer = rand() % 100;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			behaviourTimer -= dt;
			if (behaviourTimer <= 0.0f) {
				std::cout << "Found a key!\n";
				return Success;
			}
		}
		return state; // will be ongoing until success
		}
	);

	BehaviourAction* goToRoom = new BehaviourAction("Go To Room", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Going to the loot room!\n";
			state = Ongoing;
		}
		else if (state == Ongoing) {
			distanceToTarget -= dt;
			if (distanceToTarget <= 0.0f) {
				std::cout << "Reached the Room!\n";
				return Success;
			}
		}
		return state;
		}
	);

	BehaviourAction* openDoor = new BehaviourAction("Open Door", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Opening Door!\n";
			return Success;
		}
		return state;
		}
	);

	BehaviourAction* lookForTreasure = new BehaviourAction("Look For Treasure", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for treasure\n";
			return Ongoing;
		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "I found some treasure!\n";
				return Success;
			}
			std::cout << "No treasure here...\n";
			return Failure;
		}
		return state;
		}
	);

	BehaviourAction* lookForItems = new BehaviourAction("Look For Items", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for items!\n";
			return Ongoing;
		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "I found some items!\n";
				return Success;
			}
			std::cout << "No items in here...\n";
			return Failure;
		}
		return state;
		}
	);

	BehaviourSequence* sequence = new BehaviourSequence("Room Sequence");
	sequence->AddChild(findKey);
	sequence->AddChild(goToRoom);
	sequence->AddChild(openDoor);

	BehaviourSelector* selection = new BehaviourSelector("Loot Selection");
	selection->AddChild(lookForTreasure);
	selection->AddChild(lookForItems);

	BehaviourSequence* rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(sequence);
	rootSequence->AddChild(selection);

	for (int i = 0; i < 5; i++) {
		rootSequence->Reset();
		behaviourTimer = 0.0f;
		distanceToTarget = rand() % 250;
		BehaviourState state = Ongoing;
		std::cout << "We're going on an adventure!\n";
		while (state == Ongoing) {
			state = rootSequence->Execute(0.05f); // fake dt
		}

		if (state == Success) {
			std::cout << "What a successfull adventure!\n\n\n";
		}
		else if (state == Failure) {
			std::cout << "What a wast of time!\n\n\n";
		}
	}
	std::cout << "All done!\n\n\n";
}

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
	
	TestPathfinding();
	//TestBehaviourTree();

	TutorialGame* g = new TutorialGame();
	LevelOne* f = new LevelOne();
	LevelTwo* h = new LevelTwo();
	MainMenu* m = new MainMenu();
	PushdownMachine p = new Menu(m, g, f, h);
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