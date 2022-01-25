#pragma once
#include "GameObject.h"
#include "../../Common/Vector3.h"

namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class Pendulum : public GameObject {
		public:
			Pendulum();
			~Pendulum();

			virtual void Update(float dt);

		protected:
			void MoveLeft(float dt);
			void MoveRight(float dt);

			StateMachine* stateMachine;
			float counter;
		};
	}
}