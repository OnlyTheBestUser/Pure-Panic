#pragma once
#include "../../Common/Vector3.h"
#include "../../Common/Matrix3.h"

using namespace NCL::Maths;

namespace NCL {
	class CollisionVolume;
	
	namespace CSC8503 {
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

		protected:
			const CollisionVolume* volume;
			Transform*		transform;

			float inverseMass;
			float elasticity;
			float friction;

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
		};
	}
}

