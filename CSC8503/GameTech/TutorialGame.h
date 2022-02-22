#pragma once
#include "Renderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/Player.h"

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
			void SetState(GameState s) { state = s; }
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
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Maths::Vector3& cubeDims);
			void InitDefaultFloor();
	
			bool SelectObject();
			void MoveSelectedObject(float dt);
			void DebugObjectMovement();
			void DebugDrawCollider(const CollisionVolume* c, Transform* worldTransform);
			void DebugDrawVelocity(const Vector3& vel, Transform* worldTransform);
			void DebugDrawObjectInfo(const GameObject* obj);

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, bool rubber = false, bool hollow = false, bool dynamic = false);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool OBB = false, float inverseMass = 10.0f, int layer = 1, bool isTrigger = false, bool dynamic = false);

			void AddLongWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture);
			GameObject* AddAABBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name = "AABBWall");
			GameObject* AddRenderPartToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture);

			GameObject* AddWallToWorld(const Vector3& position, Vector3 dimensions, int rotation);
			GameObject* AddOBBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name = "");
			void AddCornerWallToWorld(const Vector3& position, Vector3 dimensions, int rotation);
			void AddSecurityCameraToWorld(const Vector3& position, int rotation);
			void AddWallHammerToWorld(const Vector3& position, Vector3 dimensions, int rotation);
			
			GameObject* AddCapsuleToWorld(const Maths::Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			Player* AddPlayerToWorld(const Vector3& position);

			StateGameObject* testStateObject;

			Renderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			GameState state;

			void UpdateGameWorld(float dt);
			void UpdatePauseScreen(float dt);
			void UpdateWinScreen(float dt);

			bool useGravity;
			bool inSelectionMode;
			bool debugDraw;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			MeshGeometry*	capsuleMesh = nullptr;
			MeshGeometry*	cubeMesh	= nullptr;
			MeshGeometry*	sphereMesh	= nullptr;

			MeshGeometry*	corridorFloor = nullptr;
			TextureBase*	corridorFloorTex = nullptr;
			MeshGeometry*	corridorWallAlert = nullptr;
			TextureBase*	corridorWallAlertTex = nullptr;
			MeshGeometry*	corridorWallCorner = nullptr;
			TextureBase*	corridorWallCornerTex = nullptr;
			MeshGeometry*	corridorWallLight = nullptr;
			TextureBase*	corridorWallLightTex = nullptr;
			MeshGeometry*	securityCamera = nullptr;
			TextureBase*	securityCameraTex = nullptr;
			MeshGeometry*	corridorWallScreen = nullptr;
			TextureBase*	corridorWallScreenTex = nullptr;
			MeshGeometry*	corridorWallStraight = nullptr;
			TextureBase*	corridorWallStraightTex = nullptr;
			MeshGeometry*	corridorWallHammer = nullptr;
			TextureBase*	corridorWallHammerTex = nullptr;

			TextureBase* basicTex	= nullptr;
			ShaderBase*	basicShader = nullptr;

			TextureBase* playerTex	= nullptr;

			//Coursework Meshes
			MeshGeometry*	charMeshA	= nullptr;
			MeshGeometry*	charMeshB	= nullptr;
			MeshGeometry*	enemyMesh	= nullptr;
			MeshGeometry*	bonusMesh	= nullptr;
			
			bool won = false;
			Player* player1 = nullptr;
		};
	}
}

