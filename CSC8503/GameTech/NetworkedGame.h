#pragma once
#include "TutorialGame.h"
#include "../CSC8503Common/NetworkObject.h"

namespace NCL {
	namespace CSC8503 {
		class GameServer;
		class GameClient;
		class NetworkPlayer;

#ifndef _ORBIS
		class NetworkedGame : public TutorialGame, public PacketReceiver {
#else
		class NetworkedGame : public TutorialGame {
#endif
		public:
			NetworkedGame();
			~NetworkedGame();

			void StartAsServer();
			void StartAsClient(char a, char b, char c, char d);

			void UpdateGame(float dt) override;

			void SpawnPlayer();

			void StartLevel();

#ifndef _ORBIS
			void ReceivePacket(int type, GamePacket* payload, int source) override;
#endif

			void OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b);

			void RemovePlayerFromServer(int clientID);

			static NetworkedGame* GetInstance() { return instance; }

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);

			void BroadcastSnapshot();

			static NetworkedGame* instance;
			std::map<int, int> stateIDs;

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;
			int packetsToSnapshot;
			int clientLastPacketID = 0;
			int playerID;

			std::vector<NetworkObject*> networkObjects;

			// client ID, last ID
			std::map<int, int> clientHistory;

			// client IDs, GameObjects
			std::map<int, Player*> serverPlayers;
			Player* localPlayer;

			// Packet Handling Functions
#ifndef _ORBIS
			void HandleClientPacket(ClientPacket* packet);
#endif
			void AddNewPlayerToServer(int clientID, int lastID);
			void Fire(GameObject* owner, float pitch, int clientID);

#ifndef _ORBIS
			bool CheckExists(IDPacket* packet);

			void HandleFullState(FullPacket* packet);
			void HandleFireState(FirePacket* packet);
			void HandleAssignID(AssignIDPacket* packet);
			void HandlePlayerConnect(NewPlayerPacket* packet);
			void HandlePlayerDisconnect(PlayerDisconnectPacket* packet);
			void HandlePowerUp(PowerUpPacket* packet);
#endif
		};
	}
}

