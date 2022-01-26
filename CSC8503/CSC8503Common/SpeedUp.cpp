#include "SpeedUp.h"
#include "Player.h"
#include "StateAI.h"

void NCL::CSC8503::SpeedUp::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetName() == "player") {
		if(isActive)
			((Player*)otherObject)->IncreaseSpeed(speed, time);
		isActive = false;
	}

	if (otherObject->GetName() == "enemy") {
		if (isActive)
			((StateAI*)otherObject)->IncreaseSpeed(speed, time);
		isActive = false;
	}
}
