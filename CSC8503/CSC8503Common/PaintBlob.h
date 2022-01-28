#pragma once
#include "GameObject.h"
#include <iostream>
#include "../CSC8503Common/SphereVolume.h"
#define _USE_MATH_DEFINES
#include <math.h>
namespace NCL {
	namespace CSC8503 {
		class PaintBlob : public GameObject {
		public:
			PaintBlob(std::string name = "Paint") : GameObject(name) {};

			virtual void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				std::cout << GetOverlap((PaintBlob*)otherObject) <<"\n";
			}

			float GetOverlap(PaintBlob* otherBlob) {
				//Based on maths from: https://mathworld.wolfram.com/Sphere-SphereIntersection.html
				float areaOfOverlap;
				float distBetween = (GetTransform().GetPosition() - otherBlob->GetTransform().GetPosition()).Length();
				float radius1 = ((SphereVolume*)GetBoundingVolume())->GetRadius();
				float radius2 = ((SphereVolume*)otherBlob->GetBoundingVolume())->GetRadius();

				areaOfOverlap = ((distBetween * distBetween) - ((radius1 * radius1) + (radius2 * radius2))) / (2 * (distBetween));
				return areaOfOverlap;

			}

			static float GetOverlap(PaintBlob& blob1, PaintBlob& blob2) {
				//Based on maths from: https://mathworld.wolfram.com/Sphere-SphereIntersection.html
				float areaOfOverlap;
				float distBetween = (blob1.GetTransform().GetPosition() - blob2.GetTransform().GetPosition()).Length();
				float radius1 = ((SphereVolume*)blob1.GetBoundingVolume())->GetRadius();
				float radius2 = ((SphereVolume*)blob2.GetBoundingVolume())->GetRadius();

				areaOfOverlap = ((distBetween * distBetween) - ((radius1 * radius1) + (radius2 * radius2))) / 2*(distBetween);
				return areaOfOverlap;
				
			}
		private:

		};
	}
}
