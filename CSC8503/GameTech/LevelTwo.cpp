#include "LevelTwo.h"
#include <fstream>
#include <string>
#include <iostream>
#include "../CSC8503Common/SpinningConstraint.h"


using namespace NCL;
using namespace CSC8503;

void LevelTwo::InitWorld()
{
	world->ClearAndErase();
	physics->Clear();

	GameObject* startLine = AddCubeToWorld(Vector3(30, 0.1f, 330), Vector3(10, 0.1f, 10), false, 0, 0, true);
	startLine->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	startLine->SetName("start");

	GameObject* finishLine = AddCubeToWorld(Vector3(180, 0.1f, 180), Vector3(20, 0.1f, 20), false, 0, 0, true);
	finishLine->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	finishLine->SetName("finish");

	GameObject* key = AddBonusToWorld(Vector3(330, 1, 330));
	key->SetTrigger(true);
	key->GetPhysicsObject()->SetGravity(false);
	key->SetName("key");
	key->GetRenderObject()->SetColour(Debug::BLUE);

	SpinningConstraint* s1 = new SpinningConstraint(key, Vector3(0, 0.2, 0));
	world->AddConstraint(s1);

	SpeedUp* p1 = AddSpeedUpToWorld(Vector3(50,2,330), 150.0f);
	SpeedUp* p2 = AddSpeedUpToWorld(Vector3(70,2,330), 150.0f);
	SpeedUp* p3 = AddSpeedUpToWorld(Vector3(50,2,30), 1000.0f);
	SpeedUp* p4 = AddSpeedUpToWorld(Vector3(170,2,50), 150.0f);
	powerups.emplace_back(p1);
	powerups.emplace_back(p2);
	powerups.emplace_back(p3);
	powerups.emplace_back(p4);

	player = AddPlayerBallToWorld(Vector3(30, 2, 330));
	player->SetSpawn(Vector3(30, 2, 330));
	StateAI* enemy = AddEnemyToWorld(Vector3(30, 2, 30));
	Gate* gate = AddGateToWorld(Vector3(180, 10, 210), Vector3(20, 10, 5));
	gate->GetRenderObject()->SetColour(Debug::BLUE);

	InitFloor();
	InitWalls();

	state = PLAY;

	physics->BuildStaticList();

	enemy->SetPlayer(player);
}

StateAI* NCL::CSC8503::LevelTwo::AddEnemyToWorld(Vector3 position)
{
	StateAI* enemy = new StateAI(world->GetNavGrid(), player, powerups, world);
	
	Vector3 sphereSize = Vector3(1.0f, 1.0f, 1.0f);
	SphereVolume* volume = new SphereVolume(1.0f);
	enemy->SetBoundingVolume((CollisionVolume*)volume);
	
	enemy->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	enemy->SetRenderObject(new RenderObject(&enemy->GetTransform(), sphereMesh, basicTex, basicShader));
	enemy->SetPhysicsObject(new PhysicsObject(&enemy->GetTransform(), enemy->GetBoundingVolume()));
	enemy->GetPhysicsObject()->SetInverseMass(10.0f);
	enemy->GetPhysicsObject()->InitSphereInertia();
	enemy->GetPhysicsObject()->SetElasticity(0.9f);
	enemy->GetPhysicsObject()->SetFriction(0.2f);
	enemy->SetSpawn(position);
	enemy->SetName("enemy");
	enemy->SetLayer(1);

	enemy->GetRenderObject()->SetColour(Debug::RED);
	enemy->SetDynamic(true);
	world->AddGameObject(enemy);

	return enemy;
}

Gate* NCL::CSC8503::LevelTwo::AddGateToWorld(Vector3 position, Vector3 dimensions)
{
	Gate* g = new Gate(player);
	AABBVolume* volume = new AABBVolume(dimensions);
	g->SetBoundingVolume((CollisionVolume*)volume);
	g->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);


	g->SetRenderObject(new RenderObject(&g->GetTransform(), cubeMesh, basicTex, basicShader));
	g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume()));

	g->GetPhysicsObject()->SetInverseMass(0.0f);
	g->GetPhysicsObject()->InitCubeInertia();
	g->GetPhysicsObject()->SetElasticity(2.0f);

	world->AddGameObject(g);

	g->SetLayer(0);
	g->SetTrigger(false);

	return g;
}

SpeedUp* NCL::CSC8503::LevelTwo::AddSpeedUpToWorld(Vector3 position, float speed, float time)
{
	SpeedUp* bonus = new SpeedUp(speed, time);
	SphereVolume* volume = new SphereVolume(1.0f);
	bonus->SetBoundingVolume((CollisionVolume*)volume);
	bonus->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	bonus->SetRenderObject(new RenderObject(&bonus->GetTransform(), bonusMesh, nullptr, basicShader));
	bonus->SetPhysicsObject(new PhysicsObject(&bonus->GetTransform(), bonus->GetBoundingVolume()));

	bonus->GetPhysicsObject()->SetInverseMass(1.0f);
	bonus->GetPhysicsObject()->InitSphereInertia();

	bonus->GetRenderObject()->SetColour(Vector4(1.0f, 0.84f, 0.0f, 1.0f));
	bonus->SetTrigger(true);
	bonus->GetPhysicsObject()->SetGravity(false);
	bonus->SetName("speedup");
	bonus->GetRenderObject()->SetColour(Debug::MAGENTA);
	world->AddGameObject(bonus);

	return bonus;
}

void LevelTwo::InitFloor()
{
	AddCubeToWorld(Vector3(180, -2, 180), Vector3(180, 2, 180), false, 0.0f, 0);
}

void LevelTwo::InitWalls()
{

	std::ifstream file;
	file.open("../../Assets/Data/TestGrid1.txt");

	string line;
	vector<string> lines;
	while (std::getline(file, line)) {
		lines.emplace_back(line);
	}

	for (auto x : lines) {
		std::cout << x << std::endl;
	}

	int offset = stoi(lines[0]);
	float cubeOffset = offset / 2.0f;

	for (int i = 3; i < lines.size(); ++i) {
		for (int j = 0; j < lines[3].length(); ++j) {
			if (lines[i][j] == 'x') {
				GameObject* x = AddCubeToWorld(Vector3((offset * (j)) + cubeOffset , cubeOffset, (offset * (i-3)) + cubeOffset), Vector3(1, 1, 1) * cubeOffset, false, 0.0f, 0);
				x->GetRenderObject()->SetColour(Vector4((1.0f * j) / (float)stoi(lines[1]), 0.2f, (1.0f * i) / (float)stoi(lines[2]), 1.0f));
			}
		}
	}
}
