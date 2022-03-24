#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"

#include <vector>

using std::vector;

namespace NCL {
	namespace CSC8503 {
		class NetworkObject;
		class GameObject	{
		public:
			GameObject(string name = "", float paintRad = 0);
			virtual ~GameObject();

			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}
			
			bool IsActive() const {
				return isActive;
			}
			void SetActive(bool k) { isActive = k; }

			Transform& GetTransform() {
				return transform;
			}

			NetworkObject* GetNetworkObject() const {
				return networkObject;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			void SetNetworkObject(NetworkObject* newObject) {
				networkObject = newObject;
			}

			const string& GetName() const {
				return name;
			}
			void SetName(string k) { name = k; }

			virtual void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) {
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
			}

			virtual void Interact(float dt) {
			}

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();
			virtual void Update(float dt) {};

			void SetWorldID(int newID) { worldID = newID; }
			int GetWorldID() const { return worldID; }

			void SetPaintRadius(int newRad) { paintRadius = newRad; }
			int GetPaintRadius() const { return paintRadius; }

			void SetCollisionLayers(int layers) { GetPhysicsObject()->SetCollisionLayers(layers); }
			int	GetCollisionLayers() const { return GetPhysicsObject()->GetCollisionLayers(); }

			void SetTrigger(bool x) { isTrigger = x; }
			bool IsTrigger() const { return isTrigger; }

			bool IsPaintable() const { return renderObject->GetPaintMask() != nullptr; }

		protected:
			Transform			transform;

			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;
			NetworkObject*		networkObject;

			bool	isActive;
			int		worldID;
			string	name;

			int collisionLayers = CollisionLayer::LAYER_ONE;
			bool isTrigger = false;
			float paintRadius = 0.0f;

			Vector3 broadphaseAABB;
		};
	}
}

