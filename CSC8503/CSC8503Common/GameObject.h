#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"

#include <vector>

using std::vector;

namespace NCL {
	namespace CSC8503 {

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

			const string& GetName() const {
				return name;
			}
			void SetName(string k) { name = k; }

			virtual void OnCollisionBegin(GameObject* otherObject) {
				//std::cout << "OnCollisionBegin event occured!\n";
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

			void SetWorldID(int newID) {
				worldID = newID;
			}

			int		GetWorldID() const {
				return worldID;
			}

			void SetLayer(int l) {	layer = l; }
			int GetLayer() const { return layer; }

			void SetTrigger(bool x) { isTrigger = x; }
			bool IsTrigger() const { return isTrigger; }

			bool IsDynamic() const { return dynamic; }
			void SetDynamic(bool k) { dynamic = k; }

		protected:
			Transform			transform;

			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;

			bool	isActive;
			int		worldID;
			string	name;

			bool dynamic = false;

			int layer = 0;
			bool isTrigger = false;

			Vector3 broadphaseAABB;
		};
	}
}

