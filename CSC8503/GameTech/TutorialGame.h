#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/Player.h"
#include "../CSC8503Common/AudioManager.h"
#include "../CSC8503Common/BGMManager.h"

namespace NCL {
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
			}

			bool Win() const { 
				return won;
			}

		protected:
			InputHandler* inputHandler;

			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			virtual void InitWorld();

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitCapsuleGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();
	
			bool SelectObject();
			void MoveSelectedObject(float dt);
			void DebugObjectMovement();
			void DebugDrawCollider(const CollisionVolume* c, Transform* worldTransform);
			void DebugDrawVelocity(const Vector3& vel, Transform* worldTransform);
			void DebugDrawObjectInfo(const GameObject* obj);
			void UpdateBGM();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, bool rubber = false, bool hollow = false, bool dynamic = false);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool OBB = false, float inverseMass = 10.0f, int layer = 1, bool isTrigger = false, bool dynamic = false);
			GameObject* AddWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, OGLMesh* mesh, OGLTexture* texture);
			GameObject* AddSecurityCameraToWorld(const Vector3& position, Vector3 dimensions, int rotation);
			GameObject* AddWallHammerToWorld(const Vector3& position, Vector3 dimensions, int rotation);
			
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			Player* AddPlayerToWorld(const Vector3& position);

			StateGameObject* testStateObject;

			RendererBase*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;
			AudioManager*		audio;
			BGMManager*	bgm;

			GameState state;

			void UpdateGameWorld(float dt);
			void UpdatePauseScreen(float dt);
			void UpdateWinScreen(float dt);

			bool useGravity;
			bool inSelectionMode;
			bool debugDraw;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;

			OGLMesh*	corridorFloor = nullptr;
			OGLTexture*	corridorFloorTex = nullptr;
			OGLMesh*	corridorWallAlert = nullptr;
			OGLTexture*	corridorWallAlertTex = nullptr;
			OGLMesh*	corridorWallCorner = nullptr;
			OGLTexture*	corridorWallCornerTex = nullptr;
			OGLMesh*	corridorWallLight = nullptr;
			OGLTexture*	corridorWallLightTex = nullptr;
			OGLMesh*	securityCamera = nullptr;
			OGLTexture*	securityCameraTex = nullptr;
			OGLMesh*	corridorWallScreen = nullptr;
			OGLTexture*	corridorWallScreenTex = nullptr;
			OGLMesh*	corridorWallStraight = nullptr;
			OGLTexture*	corridorWallStraightTex = nullptr;
			OGLMesh*	corridorWallHammer = nullptr;
			OGLTexture*	corridorWallHammerTex = nullptr;

			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			OGLTexture* playerTex	= nullptr;

			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;
			
			bool won = false;
			Player* player1 = nullptr;
		};
	}
}

