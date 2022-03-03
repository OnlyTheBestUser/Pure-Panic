#pragma once
#ifdef _ORBIS
#include "pad.h"
#include "../../CSC8503/CSC8503Common/InputBase.h"

namespace NCL {
	namespace PS4 {
		class PS4Input : public CSC8503::InputBase
		{
		public:
			PS4Input();
			~PS4Input(); 

			void Poll() override; 

		protected:
			void InitController();

			int32_t padHandle;
			ScePadControllerInformation padInfo;
		};
	}
}
#endif

