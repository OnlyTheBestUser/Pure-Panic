#pragma once
#include "GameObject.h"
#include "../../Common/Vector3.h"
#include <chrono>

namespace NCL {
    using namespace Maths;
    namespace CSC8503 {

        class Player : public GameObject
        {
        public:
            Player(string name = "", Vector3 ch = Vector3(0, 2, 0)) : GameObject(name), checkpoint(ch), spawnPos(ch) {};
            ~Player() {};

            void OnCollisionBegin(GameObject* other) override;
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
        };
    }
}


