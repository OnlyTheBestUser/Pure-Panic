#pragma once
#include <vector>
#include "Ray.h"
#include "CollisionDetection.h"
#include "Octree.h"
#include "NavigationGrid.h"

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

			static void AddGameObject(GameObject* o);
			static void RemoveGameObject(GameObject* o, bool andDelete = false);
			
			void RemoveGameObjectsFromWorld();
			void DeleteGameObjectsFromWorld();

			void AddConstraint(Constraint* c);
			void RemoveConstraint(Constraint* c, bool andDelete = false);

			static Camera* GetMainCamera() {
				return singleton->mainCamera;
			}

			void ShuffleConstraints(bool state) {
				shuffleConstraints = state;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			static bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false);
			static bool RaycastIgnoreObject(GameObject* obj, Ray& r, RayCollision& closestCollision, bool closestObject = false);

			virtual void UpdateWorld(float dt);

			void OperateOnContents(GameObjectFunc f);

			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			void GetConstraintIterators(
				std::vector<Constraint*>::const_iterator& first,
				std::vector<Constraint*>::const_iterator& last) const;

		protected:
			static GameWorld* singleton;

			std::vector<GameObject*> gameObjects;
			std::vector<GameObject*> toDeleteGameObjects;
			std::vector<GameObject*> toRemoveGameObjects;
			std::vector<Constraint*> constraints;

			Camera* mainCamera;

			bool	shuffleConstraints;
			bool	shuffleObjects;
			int		worldIDCounter;
		};
	}
}

