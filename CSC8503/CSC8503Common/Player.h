#pragma once
#include "GameActor.h"
#include "../../Common/Vector3.h"
#include "../../Common/Camera.h"
#include <chrono>

namespace NCL {
    using namespace Maths;
    namespace CSC8503 {

        class Player : public GameActor
        {
        public:
			Player(Camera* camera, string name = "", Vector3 ch = Vector3(0, 2, 0)) : GameActor(name), checkpoint(ch), spawnPos(ch) {
				this->camera = camera;
			};
            ~Player() {};

            void OnCollisionBegin(GameObject* other, Vector3 localA, Vector3 localB, Vector3 normal) override;
            void Update(float dt) override;

            float GetTimeTaken() const { return timeTaken; }
            int GetScore() const { return score; }
            Vector3 GetCheckpoint() const { return checkpoint; }

            bool Win() const { return finish; }
            void Reset();

			bool HasKey() const { return key; }

			void IncreaseSpeed(float speedIncrease, float duration) {
				curSpeed += speedIncrease;
				powerupTime += duration;
			};

			float GetSpeed() const { return curSpeed; }

            void SetSpawn(Vector3 l) { spawnPos = l; }

			void MoveForwards() override { GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, -1) * speed); }
			void MoveBackwards() override { GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, 1) * speed); }
			void MoveLeft() override { GetPhysicsObject()->ApplyLinearImpulse(Vector3(-1, 0, 0) * speed); }
			void MoveRight() override { GetPhysicsObject()->ApplyLinearImpulse(Vector3(1, 0, 0) * speed); }

        protected:
            bool start = false;
            bool finish = false;
            float timeTaken = 0.0f;
            int score = 0;
            Vector3 spawnPos;
            Vector3 checkpoint;
			bool key = false;
			float powerupTime = 0.0f;
			float speed = 50.0f;
			float curSpeed = 50.0f;

			Camera* camera;
        };
    }
}


