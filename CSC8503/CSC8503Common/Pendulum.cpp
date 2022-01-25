#include "Pendulum.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"

using namespace NCL;
using namespace CSC8503;

Pendulum::Pendulum()
{
	counter = 0.0f;
	stateMachine = new StateMachine();
	State* moveLeft = new State([&](float dt)->void {
		this->MoveLeft(dt);
		}
	);

	State* moveRight = new State([&](float dt)->void {
		this->MoveRight(dt);
		}
	);

	stateMachine->AddState(moveLeft);
	stateMachine->AddState(moveRight);

	stateMachine->AddTransition(new StateTransition(moveLeft, moveRight, [&]()->bool {
		return transform.GetPosition().x < -5;
		}
	));

	stateMachine->AddTransition(new StateTransition(moveRight, moveLeft, [&]()->bool {
		return transform.GetPosition().x > 5;
		}
	));
}

NCL::CSC8503::Pendulum::~Pendulum()
{
	delete stateMachine;
}

void NCL::CSC8503::Pendulum::Update(float dt)
{
	stateMachine->Update(dt);
}

void NCL::CSC8503::Pendulum::MoveLeft(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(-100, 0, 0) * dt);
}

void NCL::CSC8503::Pendulum::MoveRight(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(100,0,0) * dt);
}
