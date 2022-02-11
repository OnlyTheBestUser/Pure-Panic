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
				camLocked = true;
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

//			void MoveForwards() override { std::cout << Vector3(0, 0, -1) * (-camera->GetYaw()) << std::endl; GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, -1) * (-camera->GetYaw())); }
			void MoveForwards() override { GetPhysicsObject()->AddForce(Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, -1) * 100.0f); }
			void MoveBackwards() override { GetPhysicsObject()->AddForce(Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, 1) * 100.0f); }
			void MoveLeft() override { GetPhysicsObject()->ApplyLinearImpulse(Vector3(-1, 0, 0)); }
			void MoveRight() override { GetPhysicsObject()->ApplyLinearImpulse(Vector3(1, 0, 0)); }

			void ChangeCamLock() { camLocked = !camLocked; }

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
			bool camLocked;
        };
    }
}


