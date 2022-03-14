#pragma once
#include "GameActor.h"
#include "GameWorld.h"
#include "../../Common/Vector3.h"
#include "../../Common/Camera.h"
#include"../CSC8503Common/PowerUp.h"
#include "Projectile.h"
#include <chrono>
#include <algorithm>

namespace NCL {
    using namespace Maths;
    namespace CSC8503 {
		class LevelManager;
        class Player : public GameActor
        {
        public:
			Player(Camera* camera, string name = "", Vector3 ch = Vector3(0, 2, 0)) : GameActor(name), checkpoint(ch), spawnPos(ch), world(world){
				this->camera = camera;
				camLocked = true;
				playerID = 0;
			};
            ~Player() {};

            void OnCollisionBegin(GameObject* other, Vector3 localA, Vector3 localB, Vector3 normal) override;
            void Update(float dt) override;
			
            float GetTimeTaken() const { return timeTaken; }
            int GetScore() const { return score; }
            Vector3 GetCheckpoint() const { return checkpoint; }
			
			bool IsDead();
			void Respawn();
			
            bool Win() const { return finish; }
            void Reset();

			bool HasKey() const { return key; }

			void IncreaseSpeed(const float& speedIncrease, const float& duration) {
				if (speedIncrease <= 0 || duration <= 0) return;
				curSpeed *= speedIncrease;
				powerupTime = duration;
				currentPowerUp = PowerUpType::SpeedBoost;
			};

			void IncreaseFireRate(const float& increaseFireRateFactor, const float& duration) {
				if (increaseFireRateFactor <= 0 || duration <= 0) return;
				fireRate *= increaseFireRateFactor;
				powerupTime = duration;
				currentPowerUp = PowerUpType::FireRate;
			}

			void IncreaseHealth(const float& increaseHealthBy) {
				if (increaseHealthBy <= 0) return;
				health += increaseHealthBy;
				if (health > maxHealth) health = maxHealth;

				currentPowerUp = PowerUpType::Heal;
			}
			
			void ResetPowerUps()
			{
				fireRate = defaultFireRate;
				curSpeed = defaultCurSpeed;
				currentPowerUp = PowerUpType::None;
			}

			float GetSpeed() const { return curSpeed; }

            void SetSpawn(Vector3 l) { spawnPos = l; }

			void Move(Vector3 moveBy) override { 
				if (camLocked)
					force += Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * moveBy;// *50.0f;
				else
					camera->SetPosition(camera->GetPosition() + (Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * moveBy));
			}
			void Look(Vector2 moveBy) override {
				camera->ChangeYaw(moveBy.x);
				camera->ChangePitch(moveBy.y);
			}

			void Jump() override {
				if (!camLocked)
					camera->SetPosition(camera->GetPosition() + (Vector3(0, 1, 0) * cameraVertMult));
				else if (canJump) force += Vector3(0, 1, 0);
			}

			void Descend() override {
				if (!camLocked)
					camera->SetPosition(camera->GetPosition() - (Vector3(0, 1, 0) * cameraVertMult));
			}

			void Fire() override;

			Vector3 GetForwardVector() {
				return (Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, -1)).Normalised();
			}

			Vector3 GetCamFrontVec() {
				return ( Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(camera->GetPitch(), Vector3(1, 0, 0)) * Vector3(0, 0, -1)).Normalised();
			}

			PowerUpType GetCurrentPowerup () const {
				return currentPowerUp;
			}

			void DealDamage(float damageAmount) {
				health -= damageAmount;
				if (health < 0) health = 0;
			}

			void ChangeCamLock() { camLocked = !camLocked; }
			bool* GetCamLock() { return &camLocked; }

			bool IsFiring() { 
				bool ret = fired;
				fired = false;
				return ret;
			}

			Camera* GetCam() { return camera; }

			int GetPlayerID() const { return playerID; }
			void SetPlayerID(int x) { playerID = x; }

        protected:
			float CheckDistToGround();

			int playerID;

            bool start = false;
            bool finish = false;
            float timeTaken = 0.0f;
            int score = 0;
            Vector3 spawnPos;
            Vector3 checkpoint;
			bool key = false;
			float defaultFireRate = 0.2f;
			float defaultCurSpeed = 80.0f;
			float fireRate = 0.2f;
			float timeSincePrevShot = 0.0f;
			float powerupTime = 0.0f;
			float curSpeed = 80.0f;
			Vector3 force = Vector3(0,0,0);

			float inAirSpeed = 500.0f;
			bool canJump;

			const float maxHealth = 100.0f;
			float health = 90.0f;

			PowerUpType currentPowerUp = PowerUpType::None;

			float cameraVertMult = 0.5f;
			Camera* camera;
			GameWorld* world;
			bool camLocked;
			
			bool fired = false;
        };
    }
}