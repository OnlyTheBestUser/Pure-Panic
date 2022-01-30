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
				return GetOverlap(this, otherBlob);
			}

			static float GetOverlap(PaintBlob* blob1, PaintBlob* blob2) {
				//Calculates the area of overlap between two spheres
				//Based on maths from: https://mathworld.wolfram.com/Sphere-SphereIntersection.html
				if (blob1 && blob2) {
					float areaOfOverlap;
					float distBetween = (blob1->GetTransform().GetPosition() - blob2->GetTransform().GetPosition()).Length();
					float radius1 = ((SphereVolume*)blob1->GetBoundingVolume())->GetRadius();
					float radius2 = ((SphereVolume*)blob2->GetBoundingVolume())->GetRadius();

					float areaOfOverlap = (M_PI * pow((radius1 + radius2 - distBetween), 2) * (pow(distBetween, 2) + (2 * distBetween * radius2)
						- (3 * pow(radius2, 2)) + (2 * distBetween * radius1) + (6 * radius2 * radius1) - (3 * pow(radius1, 2)))) / (12 * distBetween);

					return areaOfOverlap;
				}

				//blob(s) invalid
				return -1;
			}
		private:

		};
	}
}
