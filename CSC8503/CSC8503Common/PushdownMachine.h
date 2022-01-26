#pragma once
#include "PushdownState.h"
#include <stack>

namespace NCL {
	namespace CSC8503 {

		class PushdownMachine
		{
		public:
			PushdownMachine(PushdownState* initialState) {
				activeState = nullptr;
				this->initialState = initialState;
			};
			~PushdownMachine() {};

			bool Update(float dt);

			void Reset() {
				std::stack<PushdownState*> empty;
				stateStack = empty;
				stateStack.push(initialState);
				activeState = initialState;
				activeState->OnAwake();
			}

		protected:
			PushdownState * activeState;
			PushdownState * initialState;

			std::stack<PushdownState*> stateStack;
		};
	}
}

