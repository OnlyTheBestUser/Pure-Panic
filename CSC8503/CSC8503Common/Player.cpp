#include "Player.h"
#include "Checkpoint.h"
#include "StateAI.h"

using namespace NCL;
using namespace CSC8503;

void Player::OnCollisionBegin(GameObject* other)
{
	if (other->GetName() == "start") {
		if (!start) {
			start = true;
		}
	}

	if (other->GetName() == "finish") {
		if (!finish) {
			finish = true;
		}
	}

	if (other->GetName() == "coin") {
		if (other->IsActive())
			score += 100;
		other->SetActive(false);
	}

	if (other->GetName() == "checkpoint") {
		checkpoint = ((Checkpoint*)other)->GetCheckpoint();
	}

	if (other->GetName() == "key") {
		key = true;
		if (other->IsActive()) {
			score += 1000;
		}
		other->SetActive(false);
	}

	if (other->GetName() == "enemy") {
		transform.SetPosition(spawnPos);
		((StateAI*)other)->ReturnToSpawn();
	}
}

void Player::Update(float dt)
{
	if (start && !finish) {
		timeTaken += dt;
	}

	if (powerupTime > 0.0f) {
		powerupTime -= dt;
	}
	else {
		curSpeed = speed;
	}
}

void Player::Reset() {
	start = false;
	finish = false;
	timeTaken = 0.0f;
	score = 0;
	checkpoint = spawnPos;
}
