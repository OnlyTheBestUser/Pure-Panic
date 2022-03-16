#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "../../Common/Camera.h"
#include <algorithm>

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld()	{
	mainCamera = new Camera();

	shuffleConstraints	= false;
	shuffleObjects		= false;
	worldIDCounter		= 0;

	navGrid = new NavigationGrid("TestGrid1.txt");
}

GameWorld::~GameWorld()	{
	delete navGrid;
}

void GameWorld::Clear() {
	gameObjects.clear();
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
	gameObjects.emplace_back(o);
	if (o->GetRenderObject() != nullptr && (o->IsPaintable())) {
		paintableObjects.emplace_back(o);
		paintableObjectScores.insert({ o, Vector2(0,0) });
	}
	o->SetWorldID(worldIDCounter++);
}

bool GameWorld::GetScoreForObject(GameObject* object, Vector2& outScore) {
	auto it = paintableObjectScores.find(object);
	if (it != paintableObjectScores.end()) {
		outScore = (*it).second;
		return true;
	}
	return false;
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	// Check if object is already added to vector, sometimes gets double added
	for (auto obj : toRemoveGameObjects)
	{
		if (o->GetWorldID() == obj->GetWorldID())
		{
			return;
		}
		if (o->GetRenderObject() != nullptr && (o->IsPaintable())) {
			paintableObjects.erase(std::remove(paintableObjects.begin(), paintableObjects.end(), o), paintableObjects.end());
			paintableObjectScores.erase(o);
		}
	}
	toRemoveGameObjects.emplace_back(o);
	if (andDelete) {
		toDeleteGameObjects.emplace_back(o);
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

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject) const {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	for (auto& i : gameObjects) {
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

bool GameWorld::RaycastIgnoreObject(GameObject* obj, Ray& r, RayCollision& closestCollision, bool closestObject) const {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	for (auto& i : gameObjects) {
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


