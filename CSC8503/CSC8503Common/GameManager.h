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
			float score = 0.1f;
			std::vector<Player*> TeamPlayers;
		};

		class GameManager
		{
		
		public:

			GameManager(TutorialGame* tg) : game(tg) {
				timer = new Timer(abs(10.0f));
			}

			void Update(const float& dt);

			void PrintScores(float scoreA, float scoreB);
			void PrintResult(float scoreA, float scoreB);

			Timer* GetTimer() const {
				return timer;
			}

			bool IsTimerFinished() {
				return timer->GetState() == TimerStates::Ended;
			}

			void UpdateScores(Vector2 scores);
			void SetScores(Vector2 scores);
			Vector2 GetScores() { return Vector2(Teams[A].score, Teams[B].score); }

			Vector2 CalcCurrentScoreRatio();

			void AddPlayerToTeam(Player* player, const TeamTypes& t) {
				Teams[t].TeamPlayers.push_back(player);
			}

			static Vector4 GetColourForID(int playerID);


			void StartRound();

			bool printResults = false;
			static Vector4 team1Colour;
			static Vector4 team2Colour;

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