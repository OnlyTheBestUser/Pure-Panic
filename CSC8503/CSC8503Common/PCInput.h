#pragma once
#ifndef _ORBIS
#include "InputBase.h"

namespace NCL {
	namespace CSC8503 {
		class PCInput : public InputBase {
		public:
			PCInput();
			~PCInput();

			void Poll() override;

		};
	}
}

#endif