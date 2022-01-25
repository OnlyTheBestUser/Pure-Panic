#include "Gate.h"

using namespace NCL;
using namespace CSC8503;

void Gate::Update(float dt)
{
	if (p->HasKey()) {
		isActive = false;
		isTrigger = true;
	}
}
