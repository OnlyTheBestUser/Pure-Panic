#pragma once
#include "GameObject.h"
#include "NetworkBase.h"
#include "NetworkState.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		struct FullPacket : public IDPacket {
			NetworkState fullState;

			FullPacket() {
				type = Full_State;
				size = sizeof(FullPacket) - sizeof(GamePacket);
			}
		};

		enum SetGameState {
			Game_Start,
			Game_Update,
			Game_Over,
			Game_Reset
		};

		struct GameStatePacket : public GamePacket {
			int		state;
			float	team1Score;
			float 	team2Score;

			GameStatePacket() {
				type = Game_State;
				size = sizeof(GameStatePacket) - sizeof(GamePacket);
			}
		};

		struct FirePacket : public IDPacket {
			float	pitch;
			bool	spread;
			int		bulletCounter;

			FirePacket() {
				type = Fire_State;
				size = sizeof(FirePacket) - sizeof(GamePacket);
			}
		};

		struct DeathPacket : public IDPacket {
			float x, y, z;

			DeathPacket() {
				type = Death_State;
				size = sizeof(DeathPacket) - sizeof(GamePacket);
			}
		};

		struct PowerUpPacket : public IDPacket {
			int worldID;

			PowerUpPacket() {
				type = PowerUp_State;
				size = sizeof(PowerUpPacket) - sizeof(GamePacket);
			}
		};

		struct ClientPacket : public GamePacket {
			int		clientID;
			int		lastID;
			float	pos[3];
			float	pitch, yaw;
			bool	firing;
			bool	spread;
			int		bulletCounter;
			bool	hasDied;

			ClientPacket() {
				type = Received_State;
				size = sizeof(ClientPacket);
			}
		};

		class NetworkObject {
		public:
			NetworkObject(GameObject& o, int id);
			virtual ~NetworkObject() {
				object.SetNetworkObject(nullptr);
				GameWorld::RemoveGameObject(&object);
			}

			//Called by clients
			virtual bool ReadPacket(GamePacket& p);
			//Called by servers
			virtual bool WritePacket(GamePacket** p);

			void UpdateStateHistory(int minID);

			int GetNetID() { return networkID; }

			GameObject& object;

		protected:

			NetworkState& GetLatestNetworkState();

			bool GetNetworkState(int frameID, NetworkState& state);

			virtual bool ReadFullPacket(FullPacket& p);
			virtual bool WriteFullPacket(GamePacket** p);

			NetworkState lastFullState;

			std::vector<NetworkState> stateHistory;

			int deltaErrors;
			int fullErrors;

			int networkID;
		};
	}
}

