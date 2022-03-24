#include "MainMenu.h"
using namespace NCL;
using namespace CSC8503;

MainMenu::MainMenu(TutorialGame* start, TutorialGame* training) 
	: renderer(RendererBase()), networkedLevel(start), trainingLevel(training) {
	pushMachine = new PushdownMachine((PushdownState*)this);
}

PushdownState::PushdownResult MainMenu::OnUpdate(float dt, PushdownState** newState) {
	renderer.Render();

	float framed = (renderer.GetFrameNumber() / 180.f);
	float size = (50.0f * abs(sin(framed))) + 40.0f;
	renderer.DrawString("Spitoon", {20,30 }, { 0.6f,0.3f,0.8f,1.0f }, { size });

	auto drawMenuOption = [=](const std::string& string, const Maths::Vector2 pos, int selection, int menuNumber) {
		if (selection == menuNumber) {
			renderer.DrawString(string, pos, { 1.0f,0.2f,0.2f,1.0f });
		}
		else {
			renderer.DrawString(string, pos, { 0.8f,0.8f,0.8f,1.0f });
		}
	};

	drawMenuOption("Start Networked Game", { 30, 50 }, selectedItem, 0);
	drawMenuOption("Start Training Game", { 30, 60 }, selectedItem, 1);
	drawMenuOption("Quit", { 30, 70 }, selectedItem, 2);

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::W)) {
		selectedItem -= 1;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::S)) {
		selectedItem += 1;
	}
	selectedItem = std::clamp(selectedItem, 0, 2);

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) {
		switch (selectedItem)
		{
		default:
			return PushdownState::PushdownResult::NoChange;
			break;
		case 0:
			*newState = new LevelState(networkedLevel);
			return PushdownState::PushdownResult::Push;
			break;
		case 1:
			*newState = new LevelState(trainingLevel);
			return PushdownState::PushdownResult::Push;
			break;
		case 2:
			return PushdownState::PushdownResult::Exit;
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