#pragma once
#include "Renderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/Player.h"

namespace NCL {
	namespace CSC8503 {
		class LevelLoader {
		public:
			LevelLoader(GameWorld* world, PhysicsSystem* physics);
			~LevelLoader();

			void LoadLevel();
			void InitGameExamples();

			Player* AddPlayerToWorld(const Vector3& position);

			GameObject* AddFloorToWorld(const Vector3& position);

			GameObject* AddWallToWorld(const Vector3& position, Vector3 dimensions, int rotation);
			GameObject* AddAABBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name = "AABBWall");
			GameObject* AddOBBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name = "OBBWall");
			GameObject* AddLongWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture);

			void AddCornerWallToWorld(const Vector3& position, Vector3 dimensions, int rotation);
			void AddSecurityCameraToWorld(const Vector3& position, int rotation);
			void AddWallHammerToWorld(const Vector3& position, int rotation);

			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, bool rubber = false, bool hollow = false, bool dynamic = false);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool OBB = false, float inverseMass = 10.0f, int layer = 1, bool isTrigger = false, bool dynamic = false);
			GameObject* AddCapsuleToWorld(const Maths::Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			GameObject* AddRenderPartToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture);

		protected:
			GameWorld*     world;
			PhysicsSystem* physics;

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
			MeshGeometry* securityCamera			= nullptr;
			TextureBase*  securityCameraTex			= nullptr;
			MeshGeometry* corridorWallScreen		= nullptr;
			TextureBase*  corridorWallScreenTex		= nullptr;
			MeshGeometry* corridorWallStraight		= nullptr;
			TextureBase*  corridorWallStraightTex	= nullptr;
			MeshGeometry* corridorWallHammer		= nullptr;
			TextureBase*  corridorWallHammerTex		= nullptr;

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