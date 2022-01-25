#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/Player.h"

namespace NCL {
	namespace CSC8503 {
		enum GameState {
			PLAY,
			PAUSE,
			WIN,
			RESET
		};

		class StateGameObject;
		class Checkpoint;
		class TutorialGame		{
		public:
			TutorialGame();
			virtual ~TutorialGame();

			virtual void UpdateGame(float dt);
			void SetState(GameState s) { state = s; }
			void ResetGame() {
				state = RESET;
			}

			bool Win() const { 
				if(player)
					return player->Win(); 
				return false;
			}

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			virtual void InitWorld();

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();
			void BridgeConstraintTest(Vector3 cubeSize = Vector3(8, 1, 2), Vector3 startPos = Vector3(-100, 200, 0));
	
			bool SelectObject();
			void MoveSelectedObject(float dt);
			void DebugObjectMovement();
			void LockedObjectMovement();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, bool rubber = false, bool hollow = false, bool dynamic = false);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool OBB = false, float inverseMass = 10.0f, int layer = 1, bool isTrigger = false, bool dynamic = false);
			
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			Player* AddPlayerBallToWorld(const Vector3& position);
			Checkpoint* AddCheckpointToWorld(const Vector3& position, Vector3 dimensions, bool OBB, float inverseMass, int layer, bool isTrigger);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);


			StateGameObject* AddStateObjectWorld(const Vector3& position);
			StateGameObject* testStateObject;

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			GameState state;

			void UpdateGameWorld(float dt);
			void UpdatePauseScreen(float dt);
			void UpdateWinScreen(float dt);

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			OGLTexture* playerTex	= nullptr;

			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			Player* player;

		};
	}
}

