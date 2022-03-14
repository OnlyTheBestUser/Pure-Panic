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
			GameObject(string name = "");
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
				//std::cout << "OnCollisionBegin event occured!\n";
				//std::cout << "Object " << worldID << " collides with " << otherObject->GetWorldID() << ". Local A: " << localA << ", Local B: " << localB << ", normal: " << normal << std::endl;
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollisionEnd event occured!\n";
			}

			virtual void Interact(float dt) {
				std::cout << "Interacting with " << name << "\n";
			}

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();
			virtual void Update(float dt) {};

			void SetWorldID(int newID) { worldID = newID; }
			int GetWorldID() const { return worldID; }

			void SetCollisionLayers(int layers) { GetPhysicsObject()->SetCollisionLayers(layers); }
			int	GetCollisionLayers() const { return GetPhysicsObject()->GetCollisionLayers(); }

			void SetTrigger(bool x) { isTrigger = x; }
			bool IsTrigger() const { return isTrigger; }

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

			Vector3 broadphaseAABB;
		};
	}
}

