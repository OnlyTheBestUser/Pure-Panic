#pragma once
#include <string>

enum BehaviourState {
	Initialise,
	Failure,
	Success,
	Ongoing
};
namespace NCL {
	namespace CSC8503 {
		class BehaviourNode {
		public:
			BehaviourNode(const std::string& nodeName) {
				currentState = Initialise;
				name = nodeName;
			}
			virtual ~BehaviourNode() {}
			virtual BehaviourState Execute(float dt) = 0;

			virtual void Reset() { currentState = Initialise; }

		protected:
			BehaviourState currentState;
			std::string name;
		};
	}
}