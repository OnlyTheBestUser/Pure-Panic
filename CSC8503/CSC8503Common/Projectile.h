#pragma once
#include "GameObject.h"

namespace NCL {

	namespace CSC8503 {

		class Projectile : public GameObject
		{
		public:
			Projectile(float capsuleRadius, float capsuleHeight): GameObject("projectile") {
				radius = capsuleRadius;
				height = capsuleHeight;
			};
		private:
			//Vector3 velocity;
			float radius;
			float height;
		};
	}
}
