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

			void RemovePlayerFromServer(int clientID);

			void UpdatePauseState(float dt) override;
			void UpdatePauseScreen(float dt) override;

			static NetworkedGame* GetInstance() { return singleton; }

			/*static void AddPowerUp(PowerUp* powerup) { singleton->powerups.emplace_back(powerup); }
			static void AddSpawnPoint(Vector3 pos) { singleton->spawnPoints.emplace_back(pos); }*/

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);

			void BroadcastSnapshot();

			static NetworkedGame* singleton;
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

			void HandleClientPacket(ClientPacket* packet);
			void AddNewPlayerToServer(int clientID, int lastID);
			void ServerFire(GameObject* owner, float pitch, int bulletCounter, bool spread, int clientID);
			void Fire(GameObject* owner, bool spread, int bulletCounter, float pitch, int clientID);

			bool CheckExists(IDPacket* packet);

			void HandleFullState(FullPacket* packet);
			void HandleFireState(FirePacket* packet);
			void HandleAssignID(AssignIDPacket* packet);
			void HandlePlayerConnect(NewPlayerPacket* packet);
			void HandlePlayerDisconnect(PlayerDisconnectPacket* packet);
			void HandlePowerUp(PowerUpPacket* packet);
		};
	}
}

