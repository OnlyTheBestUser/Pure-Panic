#pragma once
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class GameActor : public GameObject {
		public:

			GameActor(string name = "") : GameObject(name) {};
			virtual ~GameActor() {};

			virtual void MoveForwards()		{ std::cout << "Move Forwards!" << std::endl; }
			virtual void MoveBackwards()	{ std::cout << "Move Backwards!" << std::endl; }
			virtual void MoveLeft()			{ std::cout << "Move Left!" << std::endl; }
			virtual void MoveRight()		{ std::cout << "Move Right!" << std::endl; }
			virtual void Jump()				{ std::cout << "Jump!" << std::endl; }
			void Fire()				{ std::cout << "Fire!" << std::endl; }

			/*
			Sprint function?
			Other functions?
			*/

		protected:
			float speed = 1.5f;
			float jumpForce = 200.0f;

			// Potentially put camera here?
		};
	}
}