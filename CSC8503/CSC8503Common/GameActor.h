#pragma once
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class GameActor : public GameObject {
		public:

			virtual ~GameActor() {};

			void MoveForwards()		{ std::cout << "Move Forwards!" << std::endl; }
			void MoveBackwards()	{ std::cout << "Move Backwards!" << std::endl; }
			void MoveLeft()			{ std::cout << "Move Left!" << std::endl; }
			void MoveRight()		{ std::cout << "Move Right!" << std::endl; }
			void Jump()				{ std::cout << "Jump!" << std::endl; }
			void Fire()				{ std::cout << "Fire!" << std::endl; }

			/*
			Sprint function?
			Other functions?
			*/

		protected:
			float speed = 100.0f;
			float jumpForce = 200.0f;

			// Potentially put camera here?
		};
	}
}