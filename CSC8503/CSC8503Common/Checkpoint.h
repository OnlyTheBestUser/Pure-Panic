#pragma once
#include "GameObject.h"
#include "../../Common/Vector3.h"

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		class Checkpoint : public GameObject {
		public:
			Checkpoint() : GameObject("checkpoint") {};
			~Checkpoint() {};

			void SetCheckpoint(Vector3 x) { checkpoint = x; }
			Vector3 GetCheckpoint() const { return checkpoint; }

		protected:
			Vector3 checkpoint;
		};
	}
}