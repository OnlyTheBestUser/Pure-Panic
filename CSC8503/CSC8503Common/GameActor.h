#pragma once
#include "GameObject.h"
#include "../../Common/Vector2.h"
namespace NCL {
	namespace CSC8503 {
		class GameActor : public GameObject {
		public:

			GameActor(string name = "") : GameObject(name) {};
			virtual ~GameActor() {};

			virtual void Move(Vector3 moveBy) {}
			virtual void Look(Vector2 moveBy) {}
			virtual void Jump()				{}
			virtual void Descend()				{}
			virtual void Fire()				{}

		protected:
			float speed = 1.5f;
			float jumpForce = 200.0f;
		};
	}
}