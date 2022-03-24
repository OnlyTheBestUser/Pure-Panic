#pragma once
#include <thread>
#include <atomic>

#include "NetworkBase.h"

namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class GameServer : public NetworkBase {
		public:
			GameServer(int onPort, int maxClients);
			~GameServer();

			bool Initialise();
			void Shutdown();

			void SetGameWorld(GameWorld& g);

			//void ThreadedUpdate();

#ifndef ORBISNET
			bool SendGlobalPacket(int msgID);
			bool SendGlobalPacket(GamePacket& packet);

			bool SendPacketToPeer(int peerID, int msgID);
			bool SendPacketToPeer(int peerID, GamePacket& packet);
			bool SendPacketToPeer(ENetPeer* peer, GamePacket& packet);
#endif

			virtual void UpdateServer();

		protected:
			int			port;
			int			clientMax;
			int			clientCount;
			GameWorld*  gameWorld;

			//std::atomic<bool> threadAlive;

			// peerId, peer*
#ifndef ORBISNET
			std::map<int, ENetPeer*> connectedClients;
#endif

			//std::thread updateThread;

			int incomingDataRate;
			int outgoingDataRate;
		};
	}
}
