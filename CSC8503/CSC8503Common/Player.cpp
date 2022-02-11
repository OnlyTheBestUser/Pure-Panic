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

	}
}

void Player::Reset() {
}
