#pragma once
#include "Command.h"
#include <vector>

namespace NCL {

	namespace CSC8503 {
		class InputHandler {
		public:
			void HandleInput();

			void BindButtonW(Command* c)			{ buttonW_ = c; }
			void BindButtonA(Command* c)			{ buttonA_ = c; }
			void BindButtonS(Command* c)			{ buttonS_ = c; }
			void BindButtonD(Command* c)			{ buttonD_ = c; }
			void BindButtonSpace(Command* c)		{ buttonSpace_ = c; }
			void BindButtonLeftClick(Command* c)	{ buttonLeftClick_ = c; }
			void BindButtonShift(Command* c)		{ buttonShift_ = c; }
			void BindButtonG(Command* c)			{ buttonG_ = c; }
			void BindButtonJ(Command* c)			{ buttonG_ = c; }

		private:
			Command* buttonW_;
			Command* buttonA_;
			Command* buttonS_;
			Command* buttonD_;
			Command* buttonSpace_;
			Command* buttonLeftClick_;
			Command* buttonShift_;
			Command* buttonG_;
			Command* buttonJ_;

			std::vector<Command*> boundKeys;
		};
	}
}