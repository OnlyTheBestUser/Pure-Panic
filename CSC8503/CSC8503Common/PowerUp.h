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
	MultiBullet,
	None,
};

namespace NCL {
	namespace CSC8503 {

		const float REAPPEAR_AFTER_DURATION = 5.f;
		const float POWERUP_DURATION = 5.f;

		class PowerUp : public GameObject
		{
		public:
			virtual ~PowerUp() {}

			bool networked = false;

			virtual void Update(float dt) override {
				if (IsPicked) {
					reappearAfter -= dt;
					if (reappearAfter <= 0.f) {
						GetRenderObject()->SetVisibility(true);
						IsPicked = false;	
					}
				}
			}

			bool IsPickedUp() const {
				return IsPicked;
			}

		protected:
			PowerUp(PowerUpType powerType, bool networked = false) : GameObject("PowerUp"), powerUpType(powerType) {
				powerupDuration = POWERUP_DURATION;
				reappearAfter = REAPPEAR_AFTER_DURATION;
				IsPicked = false;
			}

			PowerUpType powerUpType;
			float powerupDuration;

			bool IsPicked;
			float reappearAfter;

		};
	}
}