#pragma once
#include "GameObject.h"
#include "../CSC8503Common/NavigationGrid.h"
#include "Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {

		class StateMachine;
		class StateAI : public GameObject
		{
		public:
			StateAI(NavigationGrid* navGrid, Player* player, vector<GameObject*> powerups, GameWorld* g);
			~StateAI();

			virtual void Update(float dt);

			void SetPlayer(Player* p) { player = p; }
			void SetSpawn(Vector3 k) { spawnPoint = k; }
			void ReturnToSpawn() { transform.SetPosition(spawnPoint); }

			void ChangeMoveSpeed(float k) { currentMoveSpeed = k; }

			void IncreaseSpeed(float speedIncrease, float duration) {
				currentMoveSpeed += speedIncrease;
				powerupTime += duration;
			};

		protected:
			StateMachine* rootMachine;
			StateMachine* roamMachine;
			StateMachine* guardMachine;

			void Wander(float dt);
			void ChasePlayer(float dt);
			void GetPowerUp(float dt);
			void Guard(float dt);

			void UpdatePath();

			NavigationGrid* grid;
			NavigationPath path;
			vector<Vector3> nodes;

			float refreshPathTime = 3.0f;
			float wanderTime = 10.0f;
			float currentTime = 0.0f;

			float defaultMoveSpeed = 100.0f;
			float guardSpeed = 150.0f;
			float chaseMoveSpeed = 180.0f;
			float sprintMoveSpeed = 250.0f;
			float currentMoveSpeed = 100.0f;

			Vector3 target;

			Player* player;
			Vector3 playerPos;
			
			bool playerHasKey;
			vector<GameObject*> powerups;

			Vector3 guardPoint;
			Vector3 spawnPoint;
			float range;
			bool guard = false;

			float defaultSpeed = 100.0f;
			float currentSpeed = 100.0f;
			float speedTimer = 0.0f;
			float powerupTime = 0.0f;
			int currentNodeIteration = 0;

			GameWorld* g;
		};
	}
}


