#pragma once
#include "LevelLoader.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/Player.h"
#ifndef _ORBIS
#include "../CSC8503Common/AudioManager.h"
#include "../CSC8503Common/BGMManager.h"
#endif
#include "../CSC8503Common/GameManager.h"
//#include "../CSC8503Common/Projectile.h"

namespace NCL {
	namespace PS4 {
		class InputBase;
	}
	namespace CSC8503 {
		enum class GameState {
			PLAY,
			PAUSE,
			WIN,
			RESET
		};

		class StateGameObject;
		class InputHandler;
		class Checkpoint;
		class TutorialGame		{
		public:
			TutorialGame();
			virtual ~TutorialGame();

			virtual void UpdateGame(float dt);

			void SetState(GameState s) { 
				state = s; 
				UpdateBGM(); 
			}

			bool Win() const { 
				return won;
			}

			bool GetPaused() {
				return pause;
			}

			bool GetQuit() {
				return quit;
			}

			Player* GetPlayer() {
				return player1;
			}

			void PaintObject();

			void AddPowerUp(PowerUp* powerup) { powerups.emplace_back(powerup); }
			void AddSpawnPoint(Vector3 pos) { spawnPoints.emplace_back(pos); }

		protected:
			InputHandler* inputHandler;

			void InitialiseAssets();

			void InitSounds();
			void InitCamera();


			virtual void InitWorld();

			bool SelectObject();
			void MoveSelectedObject(float dt);
			void DebugDrawCollider(const CollisionVolume* c, Transform* worldTransform);
			void DebugDrawVelocity(const Vector3& vel, Transform* worldTransform);
			void DebugDrawObjectInfo(const GameObject* obj);
			void UpdateBGM();
			void UpdateScores(float dt);
			virtual void UpdatePauseState(float dt);

			int currentObj;

			float timeSinceLastScoreUpdate;

			Renderer*			renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;
#ifndef _ORBIS
			NCL::AudioManager*	audio;
			BGMManager*			bgm;
#endif
			LevelLoader*		levelLoader;

			GameState state;
			GameManager* gameManager;

			void UpdateGameWorld(float dt);
			void UpdateDebugText(float dt);
			virtual void UpdatePauseScreen(float dt);
			void UpdateWinScreen(float dt);

			bool useGravity;
			bool inSelectionMode;
			bool debugDraw;
			bool pause = false;
			bool pausePressed = false;
			bool quit = false;

			std::vector<PowerUp*>		powerups;
			std::vector<Vector3>		spawnPoints;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			bool won = false;
			Player* player1 = nullptr;
		};
	}
}

