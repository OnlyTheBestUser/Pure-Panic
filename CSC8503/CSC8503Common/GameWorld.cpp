#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "../../Common/Camera.h"
#include <algorithm>

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld* GameWorld::singleton = nullptr;

GameWorld::GameWorld()	{
	mainCamera = new Camera();

	shuffleConstraints	= false;
	shuffleObjects		= false;
	worldIDCounter		= 0;

	singleton = this;
}

GameWorld::~GameWorld()	{
}

void GameWorld::Clear() {
	gameObjects.clear();
	paintableObjects.clear();
	constraints.clear();
}

void GameWorld::ClearAndErase() {
	for (auto& i : gameObjects) {
		delete i;
	}
	for (auto& i : constraints) {
		delete i;
	}
	Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	singleton->gameObjects.emplace_back(o);
	if (o->GetRenderObject() != nullptr && (o->IsPaintable()) && (o->GetPaintRadius() != 0)) {
		singleton->paintableObjects.emplace_back(o);
		singleton->paintableObjectScores.insert({ o, Vector2(0,0) });
	}
	o->SetWorldID(singleton->worldIDCounter++);
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	// Check if object is already added to vector, sometimes gets double added
	for (auto obj : singleton->toRemoveGameObjects)
	{
		if (o->GetWorldID() == obj->GetWorldID())
		{
			return;
		}
		if (o->GetRenderObject() != nullptr && (o->IsPaintable())) {
			singleton->paintableObjects.erase(std::remove(singleton->paintableObjects.begin(), singleton->paintableObjects.end(), o), singleton->paintableObjects.end());
			singleton->paintableObjectScores.erase(o);
		}
	}
	singleton->toRemoveGameObjects.emplace_back(o);
	if (andDelete) {
		singleton->toDeleteGameObjects.emplace_back(o);
	}
}
void GameWorld::RemoveGameObjectsFromWorld()
{
	for (auto o : toRemoveGameObjects)
		gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
}
void GameWorld::DeleteGameObjectsFromWorld()
{
	for (auto o : toDeleteGameObjects)
	{
		if (o != nullptr)
		{
			delete o;
			o = nullptr;
		}
	}
}

void GameWorld::GetObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::GetPaintableObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first = paintableObjects.begin();
	last = paintableObjects.end();
}

void GameWorld::UpdateScore(GameObject* paintableScore, Vector2 scoreChange) {
	paintableObjectScores[paintableScore] += scoreChange;
}

Vector2 GameWorld::GetScoreForObject(GameObject* paintableScore) {
	return paintableObjectScores[paintableScore];
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void GameWorld::UpdateWorld(float dt) {
	if (shuffleObjects) {
		std::random_shuffle(gameObjects.begin(), gameObjects.end());
	}

	if (shuffleConstraints) {
		std::random_shuffle(constraints.begin(), constraints.end());
	}

	for (auto x : gameObjects) {
		x->Update(dt);
	}
	
	RemoveGameObjectsFromWorld();
	DeleteGameObjectsFromWorld();
	toRemoveGameObjects.clear();
	toDeleteGameObjects.clear();

}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject) {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	for (auto& i : singleton->gameObjects) {

		if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
			continue;
		}
		bool skip = false;
		if (!((r.GetCollisionLayers() & i->GetCollisionLayers()) != 0)) { // check
			skip = true;
		}

		if (!skip) {
			RayCollision thisCollision;
			if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {
				if (!closestObject) {
					closestCollision = collision;
					closestCollision.node = i;
					return true;
				}
				else {
					if (thisCollision.rayDistance < collision.rayDistance) {
						thisCollision.node = i;
						collision = thisCollision;
					}
				}
			}
		}
	}
	if (collision.node) {
		closestCollision		= collision;
		closestCollision.node	= collision.node;
		return true;
	}
	return false;
}

bool GameWorld::RaycastIgnoreObject(GameObject* obj, Ray& r, RayCollision& closestCollision, bool closestObject) {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	for (auto& i : singleton->gameObjects) {
		if (i == obj) {
			continue;
		}
		if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
			continue;
		}
		bool skip = false;
		if (!((r.GetCollisionLayers() & i->GetCollisionLayers()) != 0)) { // check
			skip = true;
		}

		if (!skip) {
			RayCollision thisCollision;
			if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {

				if (!closestObject) {
					closestCollision = collision;
					closestCollision.node = i;
					return true;
				}
				else {
					if (thisCollision.rayDistance < collision.rayDistance) {
						thisCollision.node = i;
						collision = thisCollision;
					}
				}
			}
		}
	}
	if (collision.node) {
		closestCollision = collision;
		closestCollision.node = collision.node;
		return true;
	}
	return false;
}



/*
Constraint Tutorial Stuff
*/

void GameWorld::AddConstraint(Constraint* c) {
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
	constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
	if (andDelete) {
		delete c;
	}
}

void GameWorld::GetConstraintIterators(
	std::vector<Constraint*>::const_iterator& first,
	std::vector<Constraint*>::const_iterator& last) const {
	first	= constraints.begin();
	last	= constraints.end();
}


