#pragma once
#include "GameObject.h"
#include "../../Common/Vector2.h"
namespace NCL {
	namespace CSC8503 {
		class GameActor : public GameObject {
		public:

			GameActor(string name = "") : GameObject(name) {};
			virtual ~GameActor() {};

			virtual void Move(Vector3 moveBy) { std::cout << "MOVE" << std::endl; }
			virtual void Look(Vector2 moveBy) { std::cout << "LOOK" << std::endl; }
			virtual void Jump()				{ std::cout << "Jump!" << std::endl; }
			virtual void Descend()				{ std::cout << "Descend!" << std::endl; }
			virtual void Fire()				{ std::cout << "Fire!" << std::endl; }

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