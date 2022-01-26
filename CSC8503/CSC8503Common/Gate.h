#pragma once
#include "GameObject.h"
#include "Player.h"

namespace NCL {
    namespace CSC8503 {
        class Gate : public GameObject
        {
        public:
            Gate(Player* p) : GameObject("gate"), p(p) {};

            void Update(float dt) override;

        protected:
            Player* p;
        };
    }
}


