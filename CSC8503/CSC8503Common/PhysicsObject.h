#pragma once
#include "../../Common/Vector3.h"
#include "../../Common/Matrix3.h"
#include <queue>

using namespace NCL::Maths;

namespace NCL {
	class CollisionVolume;
	enum class VolumeType;

	namespace CSC8503 {
		enum CollisionLayer {
			LAYER_ONE = (1 << 0),
			LAYER_TWO = (1 << 1),
			LAYER_THREE = (1 << 2),
			LAYER_FOUR = (1 << 3),
			LAYER_FIVE = (1 << 4),
			LAYER_SIX = (1 << 5)
		};

		class Transform;

		class PhysicsObject	{
		public:
			PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume);
			~PhysicsObject();

			Vector3 GetLinearVelocity() const {
				return linearVelocity;
			}

			Vector3 GetAngularVelocity() const {
				return angularVelocity;
			}

			Vector3 GetTorque() const {
				return torque;
			}

			Vector3 GetForce() const {
				return force;
			}

			void SetInverseMass(float invMass) {
				inverseMass = invMass;
			}

			float GetInverseMass() const {
				return inverseMass;
			}

			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);
			
			void AddForce(const Vector3& force);
			void AddAcceleration(const Vector3& acceleration);

			void AddForceAtPosition(const Vector3& force, const Vector3& position);

			void AddTorque(const Vector3& torque);


			void ClearForces();

			void SetLinearVelocity(const Vector3& v) {
				linearVelocity = v;
			}

			void SetAngularVelocity(const Vector3& v) {
				angularVelocity = v;
			}

			void InitCubeInertia();
			void InitSphereInertia();
			void InitHollowSphereInertia();

			void UpdateInertiaTensor();

			Matrix3 GetInertiaTensor() const {
				return inverseInteriaTensor;
			}

			void SetElasticity(float p) { elasticity = p; }
			float GetElasticity() const { return elasticity; }

			void SetFriction(float f) { friction = f; }
			float GetFriction() const { return friction; }

			bool UsesGravity() const { return useGravity; }
			void SetGravity(bool k) { useGravity = k; }

			void SetSpringRes(bool k) { useSpringResolution = k; }
			bool UseSpringRes() const { return useSpringResolution; }

			void SetFriction(bool k) { useFriction = k; }
			bool UsesFriction() const { return useFriction; }

			inline void Wake() { sleeping = false; }
			inline void Sleep() { sleeping = true; }
			inline bool isSleeping() const { return sleeping; }
			
			VolumeType GetVolumeType() const;

			inline void AddToPreviousVelocities(float velocityDotProduct) { previousVelocityDotProducts.push(velocityDotProduct); }
			inline void RemoveFromPreviousVelocities() { previousVelocityDotProducts.pop(); }
			inline int GetPrevVelocitiesSize() const { return previousVelocityDotProducts.size(); }
			std::queue<float> GetPrevVelocities() const { return previousVelocityDotProducts; }
			
			inline void AddToPreviousPositions(float position) { previousPositions.push(position); }
			inline void RemoveFromPreviousPositions() { previousPositions.pop(); }
			inline int GetPreviousPositionsSize() const { return previousPositions.size(); }
			std::queue<float> GetPreviousPositions() const { return previousPositions; }

			inline Transform* GetTransform() const { return transform; }
      
			void SetCollisionLayers(int layers) { collisionLayers = layers; }
			int	GetCollisionLayers() const { return collisionLayers; }

		protected:
			const CollisionVolume* volume;
			Transform*		transform;

			float inverseMass;
			float elasticity;
			float friction;

			bool sleeping = false;
			std::queue<float> previousVelocityDotProducts;
			std::queue<float> previousPositions;

			bool useGravity = true;
			bool useFriction = true;

			//linear stuff
			Vector3 linearVelocity;
			Vector3 force;
			
			bool useSpringResolution = false;

			//angular stuff
			Vector3 angularVelocity;
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inverseInteriaTensor;

			int collisionLayers = 0;
		};
	}
}

