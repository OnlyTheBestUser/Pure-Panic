#pragma once
#include "GameActor.h"
#include "GameWorld.h"
#include "../../Common/Vector3.h"
#include <chrono>

enum PowerUpType
{
	SpeedBoost,
	FireRate,
	MuiltipleBullets
};

namespace NCL {
	namespace CSC8503 {
		class PowerUp : public GameObject
		{
		public:
			~PowerUp() {}

			//virtual void GetAbility() const = 0;
		protected:
			
			PowerUp(PowerUpType powerType) : GameObject("PowerUp"), powerUpType(powerType) {
				powerupDuration = 10.0f;
			}

			PowerUpType powerUpType;
			float powerupDuration;

		};
	}
}