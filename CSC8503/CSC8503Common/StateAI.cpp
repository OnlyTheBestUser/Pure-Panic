#include "StateAI.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "SpeedUp.h"
#include "../../Common/Vector3.h"
#include "Debug.h"
#include "Ray.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

StateAI::StateAI(NavigationGrid* navGrid, Player* player, vector<GameObject*> powerups, GameWorld* g) : g(g)
{
	this->player = player;
	this->powerups = powerups;
	playerPos = player->GetTransform().GetPosition();
	playerHasKey = player->HasKey();
	grid = navGrid;
	guardPoint = Vector3(180, 1, 230);
	if (powerups.size() > 0)
		target = powerups[0]->GetTransform().GetPosition();
	else
		target = Vector3(5000, 5000, 5000);
	range = 60.0f;

	rootMachine = new StateMachine();
	roamMachine = new StateMachine();
	guardMachine = new StateMachine();

#pragma region root level
	// Root level
	State* groupA_keyNotFound = new State([&](float dt)->void {
		//std::cout << "Roaming..." << std::endl;
		roamMachine->Update(dt);
		}
	);

	State* groupB_keyFound = new State([&](float dt)->void {
		//std::cout << "Guarding..." << std::endl;
		guardMachine->Update(dt);
		}
	);

	StateTransition* stateGroupAGroupB = new StateTransition(groupA_keyNotFound, groupB_keyFound, [&](void)->bool {
		return playerHasKey;
		});

	StateTransition* stateGroupBGroupA = new StateTransition(groupB_keyFound, groupA_keyNotFound, [&](void)->bool {
		return !playerHasKey;
	});

	rootMachine->AddState(groupA_keyNotFound);
	rootMachine->AddState(groupB_keyFound);

	rootMachine->AddTransition(stateGroupAGroupB);
	rootMachine->AddTransition(stateGroupBGroupA);

#pragma endregion

#pragma region Group A - Key Not Found

	State* wander = new State([&](float dt)->void {
		Wander(dt);
		});

	State* chasePlayer = new State([&](float dt)->void {
		ChasePlayer(dt);
		});

	State* getPowerUp = new State([&](float dt)->void {
		GetPowerUp(dt);
		});

	StateTransition* stateWanderChasePlayer = new StateTransition(wander, chasePlayer, [&](void)->bool {
		return (playerPos - this->transform.GetPosition()).Length() < range;
		});

	StateTransition* stateChasePlayerWander = new StateTransition(chasePlayer, wander, [&](void)->bool {
		return (playerPos - this->transform.GetPosition()).Length() > range + 20.0f;
		});

	StateTransition* stateChasePlayerPowerUp = new StateTransition(chasePlayer, getPowerUp, [&](void)->bool {
		return (playerPos - this->transform.GetPosition()).Length() > (target - transform.GetPosition()).Length();
		});

	StateTransition* statePowerUpChasePlayer = new StateTransition(getPowerUp, chasePlayer, [&](void)->bool {
		return (playerPos - this->transform.GetPosition()).Length() < (target - transform.GetPosition()).Length();
		});

	StateTransition* statePowerUpWander = new StateTransition(getPowerUp, wander, [&](void)->bool {
		return (playerPos - this->transform.GetPosition()).Length() > range || (target - transform.GetPosition()).Length() < 1.0f;
		});

	StateTransition* stateWanderPowerUp = new StateTransition(wander, getPowerUp, [&](void)->bool {
		return (target - this->transform.GetPosition()).Length() < range;
		});

	roamMachine->AddState(wander);
	roamMachine->AddState(chasePlayer);
	roamMachine->AddState(getPowerUp);

	roamMachine->AddTransition(stateWanderChasePlayer);
	roamMachine->AddTransition(stateChasePlayerWander);
	roamMachine->AddTransition(stateChasePlayerPowerUp);
	roamMachine->AddTransition(statePowerUpChasePlayer);
	roamMachine->AddTransition(statePowerUpWander);
	roamMachine->AddTransition(stateWanderPowerUp);
#pragma endregion
	
#pragma region Group B - Key Found

	State* guard = new State([&](float dt)->void {
		Guard(dt);
		});

	State* chasePlayer2 = new State([&](float dt)->void {
		ChasePlayer(dt);
		});

	StateTransition* stateGuardChasePlayer = new StateTransition(guard, chasePlayer2, [&](void)->bool {
		return (playerPos - this->transform.GetPosition()).Length() < 30.0f;
		});

	StateTransition* stateChasePlayerGuard = new StateTransition(chasePlayer2, guard, [&](void)->bool {
		return (playerPos - this->transform.GetPosition()).Length() > 50.0f;
		});

	guardMachine->AddState(guard);
	guardMachine->AddState(chasePlayer2);

	guardMachine->AddTransition(stateGuardChasePlayer);
	guardMachine->AddTransition(stateChasePlayerGuard);
#pragma endregion

	// States
	// Heirarchal Group A - Key not found
	// State Wander through maze

	// Chase player
	// Go to powerup

	// Heirarchal Group B - key found
	// Guard gate
	// Chase player

	// Transitions
	// Heirarchal Group A -> B	: Key found
	
	// Wander -> Chase Player		: Player within range
	// Chase Player -> Wander		: Player out of range

	// Wander -> Get powerup		: Powerup within certain range
	// Get powerup -> Wander		: Powerup gone

	// Chase player -> Get powerup	: Powerup within certain range
	// Get powerup -> Chase Player	: Power up gone / Player within certain range

	// Guard -> Chase Player		: Player within range
	// Chase Player -> Guard		: Player out of range
}

StateAI::~StateAI()
{
	delete rootMachine;
	delete roamMachine;
	delete guardMachine;
}

void StateAI::Update(float dt)
{
	if (powerupTime > 0.0f) {
		powerupTime -= dt;
	}
	else {
		currentMoveSpeed = defaultMoveSpeed;
	}

	playerPos = player->GetTransform().GetPosition();
	playerHasKey = player->HasKey();

	Vector3 direction = (playerPos - transform.GetPosition());
	if (direction.Length() < 20.0f) {
		physicsObject->AddForce(direction.Normalised() * 1000.0f * dt);
	}
	
	currentTime -= dt;

	if (currentTime <= 0.0f) {
		float shortest = FLT_MAX;
		bool active = false;
		for (auto i : powerups) {
			if (!i->IsActive())
				continue;
			active = true;
			float current = (transform.GetPosition() - i->GetTransform().GetPosition()).Length();
			if (current < shortest) {
				shortest = current;
				target = i->GetTransform().GetPosition();
			}
		}
		if (!active) // non are active
			target = Vector3(5000, 5000, 5000);
	}

	rootMachine->Update(dt);

	if (nodes.size() > 0) {
		if ((transform.GetPosition() - nodes[currentNodeIteration]).Length() < 5.0f) {
			if (currentNodeIteration < nodes.size() - 1)
				currentNodeIteration++;
		}
		else {
			Vector3 direction = (nodes[currentNodeIteration] - transform.GetPosition()).Normalised();
			physicsObject->AddForce(direction * currentMoveSpeed * dt);
		}

		for (int i = 1; i < nodes.size(); ++i) {
			Vector3 a = nodes[i - 1];
			Vector3 b = nodes[i];

			Debug::DrawLine(a + Vector3(0, 5, 0), b + Vector3(0, 5, 0), Vector4(0, 1, 0, 1));
		}
	}

}

void StateAI::Wander(float dt)
{
	//std::cout << "Wander" << std::endl;
	if(powerupTime < 0.0f)
		currentMoveSpeed = defaultMoveSpeed;
	if (nodes.size() == 0 || (nodes.back() - this->transform.GetPosition()).Length() < 5.0f) {
		// Pick a new point
		bool found = false;
		while (!found) {
			path.Clear();
			float x = (rand() % 360);
			float z = (rand() % 360);
			Vector3 newPoint(x, 0.0f, z);
			Vector3 currentPos = this->transform.GetPosition();
			currentPos.y = 0.0f;
			found = grid->FindPath(currentPos, newPoint, path);

			if (found == true)
				bool a = true;
		}

		UpdatePath();
	}


}

void StateAI::ChasePlayer(float dt)
{
	guard = false;
	if (powerupTime < 0.0f)
		currentMoveSpeed = chaseMoveSpeed;
	if (currentTime < 0.0f) {
		currentTime = refreshPathTime;
		// Pick new point
		path.Clear();
		grid->FindPath(transform.GetPosition(), player->GetTransform().GetPosition(), path);
		UpdatePath();
	}
}

void StateAI::GetPowerUp(float dt)
{
	if (powerupTime < 0.0f)
		currentMoveSpeed = defaultMoveSpeed;
	if (currentTime < 0.0f) {
		currentTime = refreshPathTime;
		path.Clear();
		grid->FindPath(transform.GetPosition(), target, path);
		UpdatePath();
	}
}

void StateAI::Guard(float dt)
{
	if (powerupTime < 0.0f)
		currentMoveSpeed = guardSpeed;
	if (!guard) {
		guard = true;
		path.Clear();
		grid->FindPath(transform.GetPosition(), guardPoint, path);
		UpdatePath();
	}
}

void NCL::CSC8503::StateAI::UpdatePath()
{
	nodes.clear();
	currentNodeIteration = 0;
	Vector3 pos;
	path.PopWaypoint(pos);
	while (path.PopWaypoint(pos)) {
		nodes.push_back(pos + Vector3(10, 2, 10));
	}
}
