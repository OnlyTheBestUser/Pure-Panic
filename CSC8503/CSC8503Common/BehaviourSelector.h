#pragma once
#include "BehaviourNodeWithChildren.h"

namespace NCL {
	namespace CSC8503 {
		class BehaviourSelector : public BehaviourNodeWithChildren {
		public:
			BehaviourSelector(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
			~BehaviourSelector() {}

			BehaviourState Execute(float dt) override {
				//std::cout << "Executing Selector: << name << std::endl;
				for (auto& i : childNodes) {
					BehaviourState nodeState = i->Execute(dt);
					switch (nodeState) {
						case Failure: continue;
						case Success:
						case Ongoing:
						{
							currentState = nodeState;
							return currentState;
						}
					}
				}
				return Failure;
			}
		};
	}
}