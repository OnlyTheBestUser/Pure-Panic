#include "PaintManager.h"

namespace NCL {
	namespace CSC8503 {

		PaintManager* PaintManager::inst = nullptr;

		PaintManager* PaintManager::GetInstance()
		{
			if (PaintManager::inst == nullptr) {
				PaintManager::inst == new PaintManager();
			}
			return PaintManager::inst;
		}

		PaintManager::PaintManager() {
		}

		PaintManager::~PaintManager() {
		}
	}
}