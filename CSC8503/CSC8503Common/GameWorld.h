#pragma once
#include <vector>
#include "Ray.h"
#include "CollisionDetection.h"
#include "Octree.h"
#include "NavigationGrid.h"
#include <map>

namespace NCL {
		class Camera;
		using Maths::Ray;

	namespace CSC8503 {
		class GameObject;
		class Constraint;

		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

		class GameWorld	{
		public:
			GameWorld();
			~GameWorld();

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void RemoveGameObject(GameObject* o, bool andDelete = false);
			void RemoveGameObjectsFromWorld();
			void DeleteGameObjectsFromWorld();

			void AddConstraint(Constraint* c);
			void RemoveConstraint(Constraint* c, bool andDelete = false);

			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleConstraints(bool state) {
				shuffleConstraints = state;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false) const;
			bool RaycastIgnoreObject(GameObject* obj, Ray& r, RayCollision& closestCollision, bool closestObject = false) const;

			virtual void UpdateWorld(float dt);

			void OperateOnContents(GameObjectFunc f);

			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			void GetPaintableObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			void UpdateScore(GameObject* paintableScore, Vector2 scoreChange);

			Vector2 GetScoreForObject(GameObject* paintableScore);

			Vector2 GetScoreRatio();


			void GetConstraintIterators(
				std::vector<Constraint*>::const_iterator& first,
				std::vector<Constraint*>::const_iterator& last) const;


			NavigationGrid* GetNavGrid() const { return navGrid; }

		protected:
			std::vector<GameObject*> gameObjects;
			std::vector<GameObject*> paintableObjects;
			std::map<GameObject*, Vector2> paintableObjectScores;
			std::vector<GameObject*> toDeleteGameObjects;
			std::vector<GameObject*> toRemoveGameObjects;
			std::vector<Constraint*> constraints;

			Camera* mainCamera;

			NavigationGrid* navGrid;

			bool	shuffleConstraints;
			bool	shuffleObjects;
			int		worldIDCounter;
		};
	}
}

