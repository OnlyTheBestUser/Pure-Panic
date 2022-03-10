#include "Projectile.h"
#include "../GameTech/Renderer.h"

void Projectile::Update(float dt) {
	Vector3 velocityDir = (this->GetPhysicsObject()->GetLinearVelocity()).Normalised();
	this->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(-acos(Vector3::Dot(Vector3(0, 1, 0), velocityDir)) * 180.0f / 3.14f, Vector3::Cross(velocityDir, Vector3(0, 1, 0)).Normalised())));
}

void Projectile::OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) {

	Ray ray(this->GetTransform().GetPosition(), -this->GetPhysicsObject()->GetLinearVelocity() * 10);
	//Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
	RayCollision closestCollision;
	//Debug::DrawArrow(this->GetTransform().GetPosition(), this->GetTransform().GetPosition() - this->GetPhysicsObject()->GetLinearVelocity(), Vector4(1, 1, 1, 1), 100.0f);
	if (gameWorld.Raycast(ray, closestCollision, true)) {
		auto test = ((GameObject*)closestCollision.node)->GetRenderObject();

		//Debug::DrawLine(ray.GetPosition(), ray.GetPosition() * ray.GetDirection());
		Debug::DrawSphere(closestCollision.collidedAt, 0.5, Vector4(1, 0, 0, 1), 0.f);
		if (test) {

			Vector2 texUV_a, texUV_b, texUV_c;
			Vector3 collisionPoint;
			Vector3 barycentric;
			CollisionDetection::GetBarycentricFromRay(ray, *test, texUV_a, texUV_b, texUV_c, barycentric, collisionPoint);

			// Get the uv from the ray
			//renderInst->Paint(test, barycentric, collisionPoint, texUV_a, texUV_b, texUV_c, 1, 0.2, 0.2, Vector4(0.3, 0, 0.5, 1));
		}
	}

	gameWorld.RemoveGameObject(this, true);
}