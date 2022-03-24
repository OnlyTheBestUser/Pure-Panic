#include "Projectile.h"
#include "../GameTech/Renderer.h"
#include "../CSC8503Common/AudioManager.h"
#include "../../Common/Assets.h"
#include "../CSC8503Common/GameManager.h"
#define _USE_MATH_DEFINES
#include <math.h>



using namespace NCL::CSC8503;

void Projectile::Update(float dt) {
	Vector3 velocityDir = (this->GetPhysicsObject()->GetLinearVelocity()).Normalised();
	this->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(-acos(Vector3::Dot(Vector3(0, 1, 0), velocityDir)) * 180.0f / 3.14f, Vector3::Cross(velocityDir, Vector3(0, 1, 0)).Normalised())));
  
	lifeSpan -= dt;
	if (lifeSpan < 0.0f)
	{
		GameWorld::RemoveGameObject(this, true);
	}
}

void Projectile::OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) {
#ifndef _ORBIS
	int soundToPlay = rand() % 2;
	NCL::AudioManager::GetInstance()->StartPlayingSound(Assets::AUDIODIR + (soundToPlay == 0 ? "splat_neutral_01.ogg" : "splat_neutral_02.ogg"), this->GetTransform().GetPosition());
#endif

	Ray ray(this->GetTransform().GetPosition() - this->GetPhysicsObject()->GetLinearVelocity().Normalised() * 2, this->GetPhysicsObject()->GetLinearVelocity());
	ray.SetCollisionLayers(CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_THREE);

	RayCollision closestCollision;
	if (GameWorld::Raycast(ray, closestCollision, true)) {

		RenderObject* test = ((GameObject*)closestCollision.node)->GetRenderObject();
		
		if (test) {
			if (test->GetPaintMask() != nullptr) {

				Vector3 center = this->GetTransform().GetPosition() - this->GetPhysicsObject()->GetLinearVelocity().Normalised() * 10;
				Vector4 colour = GameManager::GetColourForID(GetOwnerPlayerID());
				Quaternion rotation = this->GetTransform().GetOrientation();

				PaintCircle(center, rotation, colour, 7, 1.2f, 0.5f);
				PaintCircle(center, rotation, colour, 11, 3.0f, 0.4f);
			}
		}
	}
	
	GameWorld::RemoveGameObject(this, true);
}

void Projectile::PaintCircle(const Vector3& centre, const Quaternion& rotation, const Vector4& colour, int numberOfPoints, float circleRadius, float paintRadiusModifier)
{
	Vector3 firstpoint = centre + rotation * Vector3(3, 0, 0);
	Vector3 prevPoint = firstpoint;
	Vector3 curPoint;
	int pointNum = numberOfPoints;
	float rot = 0.0f;

	for (int i = 0; i < pointNum; i++) {
		rot += ((M_PI * 2) / pointNum);
		curPoint = centre + rotation * Vector3((cos(rot) * circleRadius), 0, (sin(rot) * circleRadius));

		//Debug::DrawLine(prevPoint, curPoint, colour, 1);

		Ray ray(curPoint, this->GetPhysicsObject()->GetLinearVelocity());
		ray.SetCollisionLayers(CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_THREE);

		RayCollision closestCollision;
		if (GameWorld::Raycast(ray, closestCollision, true)) {

			//Debug::DrawArrow(curPoint, subClosestCollision.collidedAt, Vector4(1,1,1,1), 0.1);

			if ((curPoint - closestCollision.collidedAt).Length() > 15.0f) {
				prevPoint = curPoint;
				continue;
			}
			
			RenderObject* rendObj = ((GameObject*)closestCollision.node)->GetRenderObject();

			if (rendObj) {
				if (rendObj->GetPaintMask() != nullptr) {

					Vector2 texUV_a, texUV_b, texUV_c;
					Vector3 collisionPoint;
					Vector3 barycentric;
					Vector3 norm;
					CollisionDetection::GetBarycentricFromRay(ray, *rendObj, texUV_a, texUV_b, texUV_c, barycentric, collisionPoint, norm);
					float randRad = (((GameObject*)closestCollision.node)->GetPaintRadius() + (((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f) - 1.0f) * ((GameObject*)closestCollision.node)->GetPaintRadius() * 0.25f) * paintRadiusModifier;
					renderInst->Paint(rendObj, barycentric, collisionPoint, texUV_a, texUV_b, texUV_c, randRad, 0.7, 1, colour);
				}
			}
		}



		prevPoint = curPoint;
	}
}

void Projectile::Paint()
{

}
