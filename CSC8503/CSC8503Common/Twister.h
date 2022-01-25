#pragma once
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Twister : public GameObject
		{
		public:
			Twister(string name = "") : GameObject(name) {};
			~Twister() {};

			void Interact(float dt) override;
		};
	}
}


