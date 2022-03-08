#pragma once
#include "GameActor.h"
#include "GameWorld.h"
#include "../../Common/Vector3.h"
#include <chrono>

enum PowerUpType
{
	SpeedBoost,
	FireRate,
	MultipleBullets,
	Heal
};

namespace NCL {
	namespace CSC8503 {
		class PowerUp : public GameObject
		{
		public:
			~PowerUp() {}

			//virtual void GetAbility() const = 0;
		protected:
			
			PowerUp(PowerUpType powerType, GameWorld& gw) : GameObject("PowerUp"), powerUpType(powerType), gameWorld(gw) {
				powerupDuration = 10.0f;
			}

			PowerUpType powerUpType;
			float powerupDuration;
			GameWorld& gameWorld;

		};
	}
}