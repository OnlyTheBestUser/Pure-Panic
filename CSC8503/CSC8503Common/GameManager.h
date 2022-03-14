#pragma once
#include "Player.h"
#include "Timer.h"

const int NO_OF_TEAMS = 2;

namespace NCL {
	namespace CSC8503 {
		
		enum TeamTypes
		{
			A,
			B
		};

		struct TeamInfo {
			float Score = 0.f;
			std::vector<Player*> TeamPlayers;
		};

		class GameManager
		{
		
		public:

			GameManager() {
				timer = new Timer(abs(10.0f));

				Teams[A].Score = 0.f;
				Teams[B].Score = 0.f;
			}

			void Update(const float& dt);

			Timer* GetTimer() const {
				return timer;
			}

			void AddPlayerToTeam(Player* player,const TeamTypes& t) {
				Teams[t].TeamPlayers.push_back(player);
			}

		private:

			Timer* timer;
			TeamInfo Teams[NO_OF_TEAMS];
		
		};
	}
}