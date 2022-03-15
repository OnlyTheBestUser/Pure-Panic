#pragma once
#include "GameActor.h"
#include "GameWorld.h"
#include "../../Common/Vector3.h"
#include <chrono>

enum PowerUpType
{
	SpeedBoost,
	FireRate,
	Heal,
	None,
};

namespace NCL {
	namespace CSC8503 {

		const float REAPPEAR_AFTER_DURATION = 5.f;
		const float POWERUP_DURATION = 5.f;

		class PowerUp : public GameObject
		{
		public:
			~PowerUp() {}

			virtual void Update(float dt) override {
				if (IsPicked) {
					reappearAfter -= dt;
					if (reappearAfter <= 0.f) {
						GetRenderObject()->SetVisibility(1.f);
						IsPicked = false;	
					}
				}
			}

		protected:
			PowerUp(PowerUpType powerType, GameWorld& gw) : GameObject("PowerUp"), powerUpType(powerType), gameWorld(gw) {
				powerupDuration = POWERUP_DURATION;
				reappearAfter = REAPPEAR_AFTER_DURATION;
				IsPicked = false;
			}

			PowerUpType powerUpType;
			float powerupDuration;
			GameWorld& gameWorld;

			bool IsPicked;
			float reappearAfter;

		};
	}
}