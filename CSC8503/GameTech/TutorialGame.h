#pragma once
#include "LevelManager.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/Player.h"
#include "../CSC8503Common/AudioManager.h"
#include "../CSC8503Common/BGMManager.h"
#include "..//CSC8503Common/Timer.h"
//#include "../CSC8503Common/Projectile.h"

namespace NCL {
	namespace PS4 {
		class InputBase;
	}
	namespace CSC8503 {
		enum GameState {
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

			void ResetGame() {
				state = RESET;
				quit = false;
				pause = false;
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

			void PaintObject();

		protected:
			InputHandler* inputHandler;

			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			virtual void InitWorld();

			/*void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitCapsuleGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Maths::Vector3& cubeDims);*/
	
			bool SelectObject();
			void MoveSelectedObject(float dt);
			void DebugObjectMovement();
			void DebugDrawCollider(const CollisionVolume* c, Transform* worldTransform);
			void DebugDrawVelocity(const Vector3& vel, Transform* worldTransform);
			void DebugDrawObjectInfo(const GameObject* obj);
			void UpdateBGM();

			StateGameObject* testStateObject;

			Renderer*			renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;
			NCL::AudioManager*		audio;
			BGMManager*	bgm;
			LevelManager*		LevelManager;

			GameState state;

			void UpdateGameWorld(float dt);
			void UpdatePauseScreen(float dt);
			void UpdateWinScreen(float dt);

			bool useGravity;
			bool inSelectionMode;
			bool debugDraw;
			bool pause = false;
			bool quit = false;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			bool won = false;
			Player* player1 = nullptr;

			Timer* timer;
		};
	}
}

