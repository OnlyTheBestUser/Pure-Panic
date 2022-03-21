#pragma once
#include "Renderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/Player.h"
#include "../CSC8503Common/PowerUp.h"
#include "../CSC8503Common/FireRate.h"
#include "../CSC8503Common/Heal.h"
#include "../CSC8503Common/SpeedBoost.h"
#include "../CSC8503Common/MultiBullet.h"


namespace NCL {
	namespace CSC8503 {
		class LevelLoader {
		public:
			LevelLoader(GameWorld* world, PhysicsSystem* physics, Renderer* renderer);
			~LevelLoader();

			void ReadInLevelFile(std::string filename);

			Player* AddPlayerToWorld(const Vector3& position);
			GameObject* AddDummyPlayerToWorld(const Vector3& position);

			PowerUp* AddPowerUpToWorld(const Vector3& position, const PowerUpType& ability, const float& radius = 1.0f);



			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, bool rubber = false, bool hollow = false, bool dynamic = false);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool OBB = false, float inverseMass = 10.0f, int layer = 1, bool isTrigger = false, bool dynamic = false);
			GameObject* AddCapsuleToWorld(const Maths::Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);
			
			Projectile* SpawnProjectile(Player* owner, const float& initialSpeed = 70.0f, const float& meshSize = 0.5f);
			Projectile* SpawnProjectile(GameObject* owner, float pitch, int playerID, const float& initialSpeed = 70.0f, const float& meshSize = 0.5f);

		protected:
			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddAABBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name = "AABBWall");
			GameObject* AddOBBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name = "OBBWall");
			GameObject* AddLongWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name = "LongWall");
			GameObject* AddPaintWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name = "PaintWall");

			void AddCornerWallToWorld(const Vector3& position, Vector3 dimensions, int rotation);
			void AddSecurityCameraToWorld(const Vector3& position, int rotation);
			void AddWallHammerToWorld(const Vector3& position, int rotation);

			GameObject* AddRenderPartToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture);
			GameObject* AddAssetToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture, const Vector3& phyLocation, const Vector3& phyDimensions, const float& paintRad = 3.f, const string& name = "PaintableAsset");

			void SplitStringOnDelimiter(const std::string& s, char delim, vector<std::string>& result);
			Vector3 Vec3FromStr(std::string input);

			GameWorld*     world;
			PhysicsSystem* physics;
			Renderer* renderer;

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