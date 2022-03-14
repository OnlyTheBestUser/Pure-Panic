#pragma once
#include "Renderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/Player.h"
#include "../CSC8503Common/PowerUp.h"
#include "../CSC8503Common/FireRate.h"
#include "../CSC8503Common/Heal.h"
#include "../CSC8503Common/SpeedBoost.h"


namespace NCL {
	namespace CSC8503 {
		class LevelManager {
		public:
			LevelManager(GameWorld* world, PhysicsSystem* physics, Renderer* renderer);
			~LevelManager();

			static void ReadInLevelFile(std::string filename);

			static Player*     SpawnPlayer(const Vector3& position);
			static GameObject* SpawnDummyPlayer(const Vector3& position);
			
			static Projectile* SpawnProjectile(Player* owner, const float& initialSpeed = singleton->PROJ_SPEED, const float& meshSize = singleton->PROJ_SIZE);
			static Projectile* SpawnProjectile(GameObject* owner, float pitch, int playerID, const float& initialSpeed = singleton->PROJ_SPEED, const float& meshSize = singleton->PROJ_SIZE);

		protected:
			GameObject* AddFloorToWorld    (const Vector3& position);
			GameObject* AddAABBWallToWorld (const Vector3& position, Vector3 dimensions, int rotation, string name = "AABBWall");
			GameObject* AddOBBWallToWorld  (const Vector3& position, Vector3 dimensions, int rotation, string name = "OBBWall");
			GameObject* AddLongWallToWorld (const Vector3& position, Vector3 dimensions, int rotation, string name = "LongWall");
			GameObject* AddPaintWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name = "PaintWall");

			GameObject* AddSphereToWorld   (const Vector3& position, float radius, float inverseMass = 10.0f, bool rubber = false, bool hollow = false, bool dynamic = false);
			GameObject* AddCubeToWorld	   (const Vector3& position, Vector3 dimensions, bool OBB = false, float inverseMass = 10.0f, int layer = 1, bool isTrigger = false, bool dynamic = false);
			GameObject* AddCapsuleToWorld  (const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			void AddCornerWallToWorld    (const Vector3& position, Vector3 dimensions, int rotation);
			void AddSecurityCameraToWorld(const Vector3& position, int rotation);
			void AddWallHammerToWorld    (const Vector3& position, int rotation);

			GameObject* AddRenderPartToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture);

			GameObject* AddPlayerObjectToWorld(const Vector3& position, GameObject* character);
			PowerUp*    AddPowerUpToWorld(const Vector3& position, const PowerUpType& ability, const float& radius = 1.0f);
			Projectile* AddProjectileToWorld(GameObject* owner, float pitch, int playerID, const float& initialSpeed = singleton->PROJ_SPEED, const float& meshSize = singleton->PROJ_SIZE);

			static void SplitStringOnDelimiter(const std::string& s, char delim, vector<std::string>& result);
			static Vector3 Vec3FromStr(std::string input);
			static bool BoolFromStr(std::string input);

			static LevelManager* singleton;

			const float PROJ_SPEED = 70.0f;
			const float PROJ_SIZE = 0.5f;

			GameWorld*     world;
			PhysicsSystem* physics;
			Renderer*	   renderer;

			ShaderBase*	  basicShader				= nullptr;
			TextureBase*  basicTex					= nullptr;
			TextureBase*  playerTex					= nullptr;

			MeshGeometry* corridorFloor				= nullptr;
			TextureBase*  corridorFloorTex			= nullptr;
			MeshGeometry* corridorWallAlert			= nullptr;
			TextureBase*  corridorWallAlertTex		= nullptr;
			MeshGeometry* corridorWallCorner		= nullptr;
			TextureBase*  corridorWallCornerTex		= nullptr;
			MeshGeometry* corridorWallLight			= nullptr;
			TextureBase*  corridorWallLightTex		= nullptr;
			MeshGeometry* corridorWallScreen		= nullptr;
			TextureBase*  corridorWallScreenTex		= nullptr;
			MeshGeometry* corridorWallStraight		= nullptr;
			TextureBase*  corridorWallStraightTex	= nullptr;
			MeshGeometry* corridorWallHammer		= nullptr;
			TextureBase*  corridorWallHammerTex		= nullptr;
			MeshGeometry* securityCamera			= nullptr;
			TextureBase*  securityCameraTex			= nullptr;

			MeshGeometry* sphereMesh				= nullptr;
			MeshGeometry* cubeMesh					= nullptr;
			MeshGeometry* capsuleMesh				= nullptr;

			MeshGeometry* charMeshA					= nullptr;
			MeshGeometry* charMeshB					= nullptr;
			MeshGeometry* enemyMesh					= nullptr;
			MeshGeometry* bonusMesh					= nullptr;
		};
	}
}