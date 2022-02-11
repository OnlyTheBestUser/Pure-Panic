#include "Player.h"

using namespace NCL;
using namespace CSC8503;

void Player::OnCollisionBegin(GameObject* other, Vector3 localA, Vector3 localB, Vector3 normal)
{

}

void Player::Update(float dt)
{
	if (camLocked)
	{
		camera->SetPosition(GetTransform().GetPosition() + Vector3(0, 3, 0));
		GetTransform().SetOrientation(Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)));
	}
	GetPhysicsObject()->AddForce(force.Normalised() * speed);
	force = Vector3(0, 0, 0);
}

void Player::Reset() 
{

}