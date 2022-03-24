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
		class TutorialGame;
		class LevelLoader {

		#define DEF_MASS 10.0f
		#define DEF_ELASTICITY 0.8f
		#define DEF_FRICTION 0.2f
		#define DEF_LDAMPING 0.4f

		#define PROJ_SPEED 70.0f
		#define PROJ_SIZE  0.5f

		public:
			LevelLoader(PhysicsSystem* physics, Renderer* renderer, TutorialGame* game);
			~LevelLoader();

			static void ReadInLevelFile(std::string filename);

			static Player*     SpawnPlayer(const Vector3& position);
			static GameObject* SpawnDummyPlayer(const Vector3& position);
			static GameObject* SpawnAIEnemy(const Vector3& position, GameObject* target = nullptr);

			static Projectile* SpawnProjectile(Player* owner, const bool& NeedBulletSpread, const float& initialSpeed = PROJ_SPEED, const float& meshSize = PROJ_SIZE);
			static Projectile* SpawnProjectile(GameObject* owner, const bool& NeedBulletSpread, const int bulletIndex, float pitch, int playerID, const float& initialSpeed = PROJ_SPEED, const float& meshSize = PROJ_SIZE);

		protected:
			GameObject* AddFloorToWorld    (const Vector3& position);
			GameObject* AddAABBWallToWorld (const Vector3& position, Vector3 dimensions, int rotation, string name = "AABBWall");
			GameObject* AddOBBWallToWorld  (const Vector3& position, Vector3 dimensions, int rotation, string name = "OBBWall");
			GameObject* AddLongWallToWorld (const Vector3& position, Vector3 dimensions, int rotation, string name = "LongWall");
			GameObject* AddPaintWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name = "PaintWall");

			GameObject* AddSphereToWorld (const Vector3& position, float radius, int layer, bool isTrigger = false, bool dynamic = false, bool rubber = false, bool hollow = false, float inverseMass = DEF_MASS);
			GameObject* AddCubeToWorld   (const Vector3& position, Vector3 dimensions, int layer, bool isTrigger = false, bool dynamic = false, bool OBB = false, float inverseMass = DEF_MASS);
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, int layer, bool isTrigger = false, bool dynamic = false, float inverseMass = DEF_MASS);

			void AddCornerWallToWorld    (const Vector3& position, Vector3 dimensions, int rotation);
			void AddSecurityCameraToWorld(const Vector3& position, int rotation);
			void AddThroneToWorld(const Vector3& position, int rotation, const Vector3& scale);

			GameObject* AddAssetToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture, const Vector3& phyLocation, const Vector3& phyDimensions, const float& paintRad = 3.f, const string& name = "PaintableAsset");

			GameObject* AddPillarToWorld(const Vector3& position, Vector3 dimensions, int rotation);
			
			void AddWallHammerToWorld    (const Vector3& position, int rotation);

			GameObject* AddRenderPartToWorld  (const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture);
			GameObject* AddPlayerObjectToWorld(const Vector3& position, GameObject* character);
			PowerUp*    AddPowerUpToWorld     (const Vector3& position, const PowerUpType& ability, const float& radius = 1.0f);
			Projectile* AddProjectileToWorld  (GameObject* owner, const bool& NeedBulletSpread, const int bulletIndex, float pitch, int playerID, const float& initialSpeed = PROJ_SPEED, const float& meshSize = PROJ_SIZE);
			Vector3		AddSpawnPointToWorld  (const Vector3& position);

			void SetFieldsForCube(GameObject* sphere, const Vector3& position, Vector3 dimensions, CollisionLayer layers, bool isTrigger = false, bool dynamic = false, bool OBB = false, 
				float inverseMass = DEF_MASS, float elasticity = DEF_ELASTICITY, float lDamping = DEF_LDAMPING, float friction = DEF_FRICTION);
			
			void SetFieldsForSphere(GameObject* sphere, const Vector3& position, CollisionLayer layers, float radius, bool isTrigger = false, bool dynamic = false, bool rubber = false, bool hollow = false, 
				float inverseMass = DEF_MASS, float elasticity = DEF_ELASTICITY, float lDamping = DEF_LDAMPING, float friction = DEF_FRICTION);
			
			void SetFieldsForCapsule(GameObject* sphere, const Vector3& position, CollisionLayer layers, float halfHeight, float radius, bool isTrigger = false, bool dynamic = false, 
				float inverseMass = DEF_MASS, float elasticity = DEF_ELASTICITY, float lDamping = DEF_LDAMPING, float friction = DEF_FRICTION);

			void SetMiscFields(GameObject* obj, CollisionVolume* volume, const Vector3& position, const Vector3& dimensions, bool isTrigger);
			PhysicsObject* GetPhysicsObject(Transform* transform, CollisionVolume* volume, int layers, bool dynamic, float inverseMass, float elasticity, float lDamping, float friction);
			RenderObject* GetRenderObject(Transform* transform, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader, Vector4 colour = Vector4(1,1,1,1));

			static void SplitStringOnDelimiter(const std::string& s, char delim, vector<std::string>& result);
			static Vector3 Vec3FromStr(std::string input);
			static bool BoolFromStr(std::string input);

			static LevelLoader* singleton;

			PhysicsSystem*	physics;
			Renderer*		renderer;
			TutorialGame*	game;

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
			MeshGeometry* sanctumThrone				= nullptr;
			TextureBase*  sanctumThroneTex			= nullptr;

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