#include "MainMenu.h"
using namespace NCL;
using namespace CSC8503;

MainMenu::MainMenu(TutorialGame* start, TutorialGame* training) : renderer(RendererBase()) {
	levelState = new LevelState(start);
	trainingState = new LevelState(training);
	pushMachine = new PushdownMachine((PushdownState*)this);
}

PushdownState::PushdownResult MainMenu::OnUpdate(float dt, PushdownState** newState) {
	renderer.Render();

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::W)) {
		selectedItem -= 1;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::S)) {
		selectedItem += 1;
	}
	selectedItem = std::clamp(selectedItem, 0, 2);

	auto drawMenuOption = [=](const std::string& string, const Maths::Vector2 pos, int selection, int menuNumber) {
		if (selection == menuNumber) {
			renderer.DrawString(string, pos, { 1.0f,0.2f,0.2f,1.0f });
		}
		else {
			renderer.DrawString(string, pos, { 0.8f,0.8f,0.8f,1.0f });
		}
	};

	drawMenuOption("Start Networked Game", {30,10}, selectedItem, 0);
	drawMenuOption("Start Training Game", { 30,30 }, selectedItem, 1);
	drawMenuOption("Quit", { 30,50 }, selectedItem, 2);

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1)) {
		switch (selectedItem)
		{
		default:
			return PushdownState::PushdownResult::NoChange;
			break;
		case 0:
			*newState = (LevelState*)levelState;
			return PushdownState::PushdownResult::Push;
			break;
		case 1:
			*newState = (LevelState*)trainingState;
			return PushdownState::PushdownResult::Push;
			break;
		case 2:
			return PushdownState::PushdownResult::NoChange;
			break;
		}
	}
	return PushdownState::PushdownResult::NoChange;
}

bool MainMenu::UpdateGame(float dt) {
	if (!pushMachine->Update(dt)) {
		return false;
	}
}