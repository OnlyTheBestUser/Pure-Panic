#pragma once
#include "GameActor.h"
#include "GameWorld.h"
#include "../../Common/Vector3.h"
#include "../../Common/Camera.h"
#include "Projectile.h"
#include <chrono>

namespace NCL {
    using namespace Maths;
    namespace CSC8503 {

        class Player : public GameActor
        {
        public:
			Player(Camera* camera, GameWorld& g, MeshGeometry* pm, ShaderBase* shd, string name = "", Vector3 ch = Vector3(0, 2, 0)) : GameActor(name), checkpoint(ch), spawnPos(ch), gameWorld(g), projectileMesh(pm), basicShader(shd) {
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

			void MoveForwards() override { force += Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, -1) * 100.0f; }
			void MoveBackwards() override { force += Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, 1) * 100.0f; }
			void MoveLeft() override { force += Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * 50.0f; }
			void MoveRight() override { force += Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Vector3(1, 0, 0) * 50.0f; }

			void Fire() override;

			Vector3 GetForwardVector() {
				return (Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, -1)).Normalised();
			}

			Vector3 GetCamFrontVec() {
				return ( Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(camera->GetPitch(), Vector3(1, 0, 0)) * Vector3(0, 0, -1)).Normalised();
			}

			void ChangeCamLock() { camLocked = !camLocked; }

        protected:
			float CheckDistToGround();

            bool start = false;
            bool finish = false;
            float timeTaken = 0.0f;
            int score = 0;
            Vector3 spawnPos;
            Vector3 checkpoint;
			bool key = false;
			float powerupTime = 0.0f;
			float speed = 5.0f;
			float curSpeed = 50.0f;
			Vector3 force = Vector3(0,0,0);

			Camera* camera;
			GameWorld& gameWorld;
			bool camLocked;
			
			ShaderBase* basicShader;
			MeshGeometry* projectileMesh;

		private:
			Projectile* spawnProjectile(const float& radius, const float& height, const float& initialSpeed = 25.0f);
        };
    }
}