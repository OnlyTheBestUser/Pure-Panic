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

	Ray ray(this->GetTransform().GetPosition(), this->GetPhysicsObject()->GetLinearVelocity());
	ray.SetCollisionLayers(CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_THREE);

	RayCollision closestCollision;
	if (GameWorld::Raycast(ray, closestCollision, true)) {

		RenderObject* test = ((GameObject*)closestCollision.node)->GetRenderObject();
		
		if (test) {
			if (test->GetPaintMask() != nullptr) {

				Vector2 texUV_a, texUV_b, texUV_c;
				Vector3 collisionPoint;
				Vector3 barycentric;
				Vector3 norm;
				CollisionDetection::GetBarycentricFromRay(ray, *test, texUV_a, texUV_b, texUV_c, barycentric, collisionPoint, norm);

				Vector4 colour = GameManager::GetColourForID(GetOwnerPlayerID());
				
				float randRad = ((GameObject*)closestCollision.node)->GetPaintRadius() + (((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f) - 1.0f) * ((GameObject*)closestCollision.node)->GetPaintRadius() * 0.25f;

				//renderInst->Paint(test, barycentric, collisionPoint, texUV_a, texUV_b, texUV_c, randRad, 0.7, 1, colour);

				Vector3 center = this->GetTransform().GetPosition() - this->GetPhysicsObject()->GetLinearVelocity().Normalised() * 10;

				//Vector3 center = collisionPoint + (norm * 10);
				Vector3 firstpoint = center + this->GetTransform().GetOrientation() * Vector3(3, 0, 0);
				Vector3 prevPoint = firstpoint;
				Vector3 curPoint;
				int pointNum = 10;
				float rot = 0.0f;
				for (int i = 0; i < pointNum; i++) {
					rot += ((M_PI * 2) / pointNum);
					curPoint = center + this->GetTransform().GetOrientation() * Vector3((cos(rot) * 3), 0, (sin(rot) * 3));
					
					Debug::DrawLine(prevPoint, curPoint, colour, 1);

					Ray subRay(curPoint, this->GetPhysicsObject()->GetLinearVelocity());
					subRay.SetCollisionLayers(CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_THREE);

					RayCollision subClosestCollision;
					if (GameWorld::Raycast(subRay, subClosestCollision, true)) {

						RenderObject* subTest = ((GameObject*)subClosestCollision.node)->GetRenderObject();

						if (subTest) {
							if (subTest->GetPaintMask() != nullptr) {

								Vector2 subTexUV_a, subTexUV_b, subTexUV_c;
								Vector3 subCollisionPoint;
								Vector3 subBarycentric;
								Vector3 subNorm;
								CollisionDetection::GetBarycentricFromRay(subRay, *subTest, subTexUV_a, subTexUV_b, subTexUV_c, subBarycentric, subCollisionPoint, subNorm);
								float subRandRad = (((GameObject*)subClosestCollision.node)->GetPaintRadius() + (((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f) - 1.0f) * ((GameObject*)subClosestCollision.node)->GetPaintRadius() * 0.25f) * 0.25f;

								// Get the uv from the ray
								renderInst->Paint(subTest, subBarycentric, subCollisionPoint, subTexUV_a, subTexUV_b, subTexUV_c, subRandRad, 0.7, 1, colour);
							}
						}
					}



					prevPoint = curPoint;
				}

				// Debug Rainbow Gun
				//renderInst->Paint(test, barycentric, collisionPoint, texUV_a, texUV_b, texUV_c, randRad, 0.7, 1, Vector4(static_cast <float> (rand()) / static_cast <float> (RAND_MAX) , static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), 1));
			}
		}
	}
	
	GameWorld::RemoveGameObject(this, true);
}