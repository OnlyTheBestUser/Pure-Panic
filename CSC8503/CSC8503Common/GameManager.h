#pragma once
#include "Player.h"
#include "Timer.h"

const int NO_OF_TEAMS = 2;

namespace NCL {
	namespace CSC8503 {

		class TutorialGame;
		
		enum TeamTypes
		{
			A,
			B
		};

		struct TeamInfo {
			float score = 0.0f;
			std::vector<Player*> TeamPlayers;
		};

		class GameManager
		{
		
		public:

			GameManager(TutorialGame* tg) : game(tg) {
				timer = new Timer(abs(10.0f));
			}

			void Update(const float& dt);

			Timer* GetTimer() const {
				return timer;
			}

			void UpdateScores(Vector2 scores);

			Vector2 CalcCurrentScoreRatio();

			void AddPlayerToTeam(Player* player,const TeamTypes& t) {
				Teams[t].TeamPlayers.push_back(player);
			}

			void StartRound();

		protected:

			void HandleScoresAfterRound();
			void PlacePlayersToSpawnPositions();

		private:

			TutorialGame* game;
			Timer* timer;
			TeamInfo Teams[NO_OF_TEAMS];		
		};
	}
}