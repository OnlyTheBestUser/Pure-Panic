#pragma once
#include "TutorialGame.h"
#include <chrono>

namespace NCL {
	namespace CSC8503 {

		class LevelOne : public TutorialGame
		{
		public:
			LevelOne() {
				InitWorld();
			};
			virtual ~LevelOne() {};

		protected:
			virtual void InitWorld() override;

			GameObject* AddPaddleToWorld(const Vector3& position, Vector3 dimensions, Vector3 normal, float force, float springForce, Vector3* target);
			void InitFloor();
			void AddPendulum(Vector3 anchorPosition, Vector3 headPos, float length, float halfHeight, float radius);
			GameObject* AddTwistBlock(Vector3 position, Vector3 size);

			Vector3 respawnPos = Vector3(0,0,0);
		};
	}
}


