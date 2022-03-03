#pragma once
#include "InputList.h"

namespace NCL {

	namespace CSC8503 {
		struct AXIS {
			float x;
			float y;

			AXIS() {
				x = 0.0f;
				y = 0.0f;
			}
		};

#define MAX_AXIS 127
#define MAX_BUTTONS 127

		class InputBase
		{
		public:
			InputBase();
			virtual ~InputBase();

			void ResetInput();

			AXIS*	GetAxis(unsigned int i);
			float	GetButton(unsigned int i);
			bool	GetButtonDown(unsigned int i);

			virtual void Poll() = 0;

		protected:

			AXIS	axis[MAX_AXIS];
			float   buttons[MAX_BUTTONS];
		};
	}
}
