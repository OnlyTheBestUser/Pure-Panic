#pragma once

namespace NCL {
	namespace CSC8503 {

		class Spring {
		public:
			Spring(float length, float k, float damping = 0.7f) : length(length), k(k), damping(damping) {}

			float GetLength() const { return length; }
			float GetK() const { return k; }
			float GetD() const { return damping; }

			void SetDamping(float k) { damping = k; }
		protected:
			float length;
			float k;
			float damping;
		};
	}
}

