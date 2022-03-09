#pragma once
#include "TutorialGame.h"
#include "../CSC8503Common/NetworkObject.h"

namespace NCL {
	namespace CSC8503 {
		class GameServer;
		class GameClient;
		class NetworkPlayer;

		class NetworkedGame : public TutorialGame, public PacketReceiver {
		public:
			NetworkedGame();
			~NetworkedGame();

			void StartAsServer();
			void StartAsClient(char a, char b, char c, char d);

			void UpdateGame(float dt) override;

			void SpawnPlayer();

			void StartLevel();

			void ReceivePacket(int type, GamePacket* payload, int source) override;

			void OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b);

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);

			void BroadcastSnapshot(bool deltaFrame);
			void UpdateMinimumState();
			std::map<int, int> stateIDs;

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;
			int packetsToSnapshot;
			int clientLastPacketID = 0;
			int playerID;

			std::vector<NetworkObject*> networkObjects;

			// ClientID, last ACK
			std::map<int, int> ackHistory;

			// client ID, last ID
			std::map<int, int> clientHistory;

			// client IDs, GameObjects
			std::map<int, Player*> serverPlayers;
			Player* localPlayer;

			// Packet Handling Functions
			void HandleClientPacket(ClientPacket* packet);
			void AddNewPlayerToServer(int clientID, int lastID);
		};
	}
}

