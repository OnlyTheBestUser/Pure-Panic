#pragma once
#include "../../Common/Vector2.h"
#include "../CSC8503Common/CollisionDetection.h"
#include "Debug.h"
#include <list>
#include <functional>

namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		template<class T>
		class Octree;

		template<class T>
		struct OctreeEntry {
			Vector3 pos;
			Vector3 size;
			T object;

			OctreeEntry(T obj, Vector3 pos, Vector3 size) {
				object		= obj;
				this->pos	= pos;
				this->size	= size;
			}
		};

		template<class T>
		class OctreeNode	{
		public:
			typedef std::function<void(std::list<OctreeEntry<T>>&)> OctreeFunc;
		protected:
			friend class Octree<T>;

			OctreeNode() {}

			OctreeNode(Vector3 pos, Vector3 size) {
				children		= nullptr;
				this->position	= pos;
				this->size		= size;
			}

			~OctreeNode() {
				delete[] children;
			}

			void Insert(T& object, const Vector3& objectPos, const Vector3& objectSize, int depthLeft, int maxSize, string name = "") {
				if(!CollisionDetection::AABBTest(objectPos, position, objectSize, size))
					return;

				if (children) { // Not a leaf node, just descend the tree
					for (int i = 0; i < 8; ++i) {
						children[i].Insert(object, objectPos, objectSize, depthLeft - 1, maxSize);
					}
				}
				else { // Currently a leaf node, can just expand
					contents.push_back(OctreeEntry<T>(object, objectPos, objectSize));
					if ((int)contents.size() > maxSize && depthLeft > 0) {
						if (!children)
							Split();
						// We need to reinsert the contents so far!
						for (const auto& i : contents) {
							for (int j = 0; j < 8; ++j) {
								auto entry = i;
								children[j].Insert(entry.object, entry.pos, entry.size, depthLeft - 1, maxSize);
							}
						}
						contents.clear();
					}
				}
			}

			void GetContentsAtNode(T& object, const Vector3& objectPos, const Vector3& objectSize, std::list<OctreeEntry<T>>& list) {
				if (!CollisionDetection::AABBTest(objectPos, position, objectSize, size))
					return;

				if (children) {
					for (int i = 0; i < 8; ++i) {
						children[i].GetContentsAtNode(object, objectPos, objectSize, list);
					}
				}
				else { // Currently a leaf node, can just expand
					for (auto i : contents) {
						list.push_back(i);
					}
				}
			}

			void Split() {
				Vector3 halfSize = size / 2.0f;
				children = new OctreeNode<T>[8];
				children[0] = OctreeNode<T>(position + Vector3(	-halfSize.x,  halfSize.y,  halfSize.z), halfSize);
				children[1] = OctreeNode<T>(position + Vector3(	 halfSize.x,  halfSize.y,  halfSize.z), halfSize);
				children[2] = OctreeNode<T>(position + Vector3(	-halfSize.x, -halfSize.y,  halfSize.z), halfSize);
				children[3] = OctreeNode<T>(position + Vector3(	 halfSize.x, -halfSize.y,  halfSize.z), halfSize);
				children[4] = OctreeNode<T>(position + Vector3(	-halfSize.x,  halfSize.y, -halfSize.z), halfSize);
				children[5] = OctreeNode<T>(position + Vector3( 	 halfSize.x,  halfSize.y, -halfSize.z), halfSize);
				children[6] = OctreeNode<T>(position + Vector3(	-halfSize.x, -halfSize.y, -halfSize.z), halfSize);
				children[7] = OctreeNode<T>(position + Vector3(	 halfSize.x, -halfSize.y, -halfSize.z), halfSize);
			}

			void DebugDraw(Vector4 colour, float time = 0.01f) {
				Vector3 s = size;
				
				Debug::DrawLine(position + Vector3(-s.x, s.y, s.z), position + Vector3(s.x, s.y, s.z), colour, time);
				Debug::DrawLine(position + Vector3(s.x, s.y, s.z), position + Vector3(s.x, -s.y, s.z), colour, time);
				Debug::DrawLine(position + Vector3(s.x, -s.y, s.z), position + Vector3(-s.x, -s.y, s.z), colour, time);
				Debug::DrawLine(position + Vector3(-s.x, -s.y, s.z), position + Vector3(-s.x, s.y, s.z), colour, time);

				Debug::DrawLine(position + Vector3(-s.x, s.y, -s.z), position + Vector3(s.x, s.y, -s.z), colour, time);
				Debug::DrawLine(position + Vector3(s.x, s.y, -s.z), position + Vector3(s.x, -s.y, -s.z), colour, time);
				Debug::DrawLine(position + Vector3(s.x, -s.y, -s.z), position + Vector3(-s.x, -s.y, -s.z), colour, time);
				Debug::DrawLine(position + Vector3(-s.x, -s.y, -s.z), position + Vector3(-s.x, s.y, -s.z), colour, time);

				Debug::DrawLine(position + Vector3(-s.x, s.y, s.z), position + Vector3(-s.x, s.y, -s.z), colour, time);
				Debug::DrawLine(position + Vector3(s.x, s.y, s.z), position + Vector3(s.x, s.y, -s.z), colour, time);
				Debug::DrawLine(position + Vector3(s.x, -s.y, s.z), position + Vector3(s.x, -s.y, -s.z), colour, time);
				Debug::DrawLine(position + Vector3(-s.x, -s.y, s.z), position + Vector3(-s.x, -s.y, -s.z), colour, time);

				if (children) {
					for (int i = 0; i < 8; i++) {
						children[i].DebugDraw(colour);
					}
				}
			}

			void OperateOnContents(OctreeFunc& func) {
				if (children) {
					for (int i = 0; i < 8; i++) {
						children[i].OperateOnContents(func);
					}
				}
				else {
					if (!contents.empty()) {
						func(contents);
					}
				}
			}

		protected:
			std::list< OctreeEntry<T> >	contents;

			Vector3 position;
			Vector3 size;

			OctreeNode<T>* children;
		};
	}
}


namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		template<class T>
		class Octree
		{
		public:
			Octree(Vector3 size, int maxDepth = 6, int maxSize = 5){
				root = OctreeNode<T>(Vector3(), size);
				this->maxDepth	= maxDepth;
				this->maxSize	= maxSize;
			}
			~Octree() {
			}

			void Insert(T object, const Vector3& pos, const Vector3& size, string name = "") {
				root.Insert(object, pos, size, maxDepth, maxSize, name);
			}

			void GetContentsAtNode(T object, const Vector3& objectPos, const Vector3& objectSize, std::list<OctreeEntry<T>>& list) {
				root.GetContentsAtNode(object, objectPos, objectSize, list);
			}

			void DebugDraw(Vector4 colour) {
				root.DebugDraw(colour);
			}

			void OperateOnContents(typename OctreeNode<T>::OctreeFunc  func) {
				root.OperateOnContents(func);
			}

		protected:
			OctreeNode<T> root;
			int maxDepth;
			int maxSize;
		};
	}
}