#include "SimpleAI.h"
#include "Debug.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace NCL;
using namespace CSC8503;

void SimpleAI::Update(float dt)
{
	if (target) {
		Vector3 dirToTarget = (target->GetTransform().GetPosition()) - transform.GetPosition();
		
		physicsObject->AddForce(dirToTarget.Normalised() * speed);

		float angle = acos(Vector3::Dot(dirToTarget.Normalised(), Vector3(0, 0, -1)));
		angle *= (180.0f / M_PI);
		if (dirToTarget.x > 0.0f)
			angle *= -1;

		transform.SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0,1,0), angle));

		Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + (dirToTarget.Normalised() * 3.0f));

	}
}