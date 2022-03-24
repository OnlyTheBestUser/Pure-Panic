#include "Projectile.h"
#include "../GameTech/Renderer.h"
#include "../CSC8503Common/AudioManager.h"
#include "../../Common/Assets.h"
#include "../CSC8503Common/GameManager.h"

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
	float pitch = 1.0f + (((static_cast <float> (rand()) / static_cast <float> (RAND_MAX))-0.5) * 2.0f / 5.0f);
	NCL::AudioManager::GetInstance()->StartPlayingSound(Assets::AUDIODIR + (soundToPlay == 0 ? "splat_neutral_01.ogg" : "splat_neutral_02.ogg"), this->GetTransform().GetPosition(), 1.0f, 0.0f, pitch);
#endif
	string name = otherObject->GetName();
	
	if (!(otherObject->GetName() == "Dummy" || otherObject->GetName() == "Player")) {
		Ray ray(this->GetTransform().GetPosition() - this->GetPhysicsObject()->GetLinearVelocity().Normalised(), this->GetPhysicsObject()->GetLinearVelocity());
		ray.SetCollisionLayers(CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_THREE);

		RayCollision closestCollision;
		if (GameWorld::Raycast(ray, closestCollision, true)) {
			RenderObject* test = ((GameObject*)closestCollision.node)->GetRenderObject();
		
			if (test) {
				if (test->GetPaintMask() != nullptr) {

					Vector2 texUV_a, texUV_b, texUV_c;
					Vector3 collisionPoint;
					Vector3 barycentric;
					CollisionDetection::GetBarycentricFromRay(ray, *test, texUV_a, texUV_b, texUV_c, barycentric, collisionPoint);

					Vector4 colour;

					if (!IsDeathProjectile) {
						colour = GameManager::GetColourForID(ownerPlayerID);
					}
					else {
						colour = GameManager::GetColourForID(ownerPlayerID + 1);
					}

					float randRad = ((GameObject*)closestCollision.node)->GetPaintRadius() + (((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f) - 1.0f) * ((GameObject*)closestCollision.node)->GetPaintRadius() * 0.25f;

					// Get the uv from the ray
					renderInst->Paint(test, barycentric, collisionPoint, texUV_a, texUV_b, texUV_c, randRad, 0.7, 1, colour);

					// Debug Rainbow Gun
					//renderInst->Paint(test, barycentric, collisionPoint, texUV_a, texUV_b, texUV_c, randRad, 0.7, 1, Vector4(static_cast <float> (rand()) / static_cast <float> (RAND_MAX) , static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), 1));
				}
			}
		}
	}
	else {
#ifndef _ORBIS
		int sound = (rand() % 3) + 1;
		NCL::AudioManager::GetInstance()->StartPlayingSound(Assets::AUDIODIR + "boy_whoa_0" + std::to_string(sound) + ".ogg");
#endif // !_ORBIS
	}
	
	GameWorld::RemoveGameObject(this, true);
}