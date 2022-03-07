#include "Projectile.h"

void Projectile::Update(float dt) {
	Vector3 velocityDir = (this->GetPhysicsObject()->GetLinearVelocity()).Normalised();
	//-acos(Vector3::Dot(Vector3(0, 1, 0), velocityDir)) * 180.0f / 3.14f

	this->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(-acos(Vector3::Dot(Vector3(0, 1, 0), velocityDir)) * 180.0f / 3.14f, RotationAxis)));
		//* Quaternion(Matrix3::Rotation(0, velocityDir)));
	//std::cout << (-acos(Vector3::Dot(ShootDir, velocityDir)) * 180.0f / 3.14f) << std::endl;
	//std::cout << InitialShootAngle << std::endl;

	//Debug::DrawArrow(GetTransform().GetPosition(), GetTransform().GetPosition() + ShootDir * 10.0f, Debug::RED, 10.0f);
	//Debug::DrawArrow(GetTransform().GetPosition(), GetTransform().GetPosition() + ShootDir * 10.0f, Debug::RED);
}