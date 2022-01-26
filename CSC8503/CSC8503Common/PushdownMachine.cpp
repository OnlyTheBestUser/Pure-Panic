#include "PushdownMachine.h"
using namespace NCL::CSC8503;

bool PushdownMachine::Update(float dt) {
	if (activeState) {
		PushdownState* newState = nullptr;
		PushdownState::PushdownResult result = activeState->OnUpdate(dt, &newState);

		switch (result) {
			case PushdownState::Pop: {
				activeState->OnSleep();
				delete activeState;
				stateStack.pop();
				if (stateStack.empty()) {
					return false;
				}
				else {
					activeState = stateStack.top();
					activeState->OnAwake();
				}
			}break;
			case PushdownState::Push: {
				activeState->OnSleep();
				stateStack.push(newState);
				activeState = newState;
				newState->OnAwake();
			}break;
			case PushdownState::Reset: {
				Reset();
			} break;
			case PushdownState::Exit: {
				return false;
			}
		}
	}
	else {
		stateStack.push(initialState);
		activeState = initialState;
		activeState->OnAwake();
	}
	return true;
}