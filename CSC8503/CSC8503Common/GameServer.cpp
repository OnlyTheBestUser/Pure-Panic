
#include "GameServer.h"
#include "GameWorld.h"
#include "../GameTech/NetworkedGame.h"
#include <iostream>

using namespace NCL;
using namespace CSC8503;

GameServer::GameServer(int onPort, int maxClients) {
	port = onPort;
	clientMax = maxClients;
	clientCount = 0;
#ifndef ORBISNET
	netHandle = nullptr;
#endif

	Initialise();
}

GameServer::~GameServer() {
	Shutdown();
}

void GameServer::Shutdown() {
#ifndef ORBISNET
	SendGlobalPacket(BasicNetworkMessages::Shutdown);
	
	enet_host_destroy(netHandle);
	netHandle = nullptr;
#endif
}

bool GameServer::Initialise() {
#ifndef ORBISNET
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;

	netHandle = enet_host_create(&address, clientMax, 1, 0, 0);

	if (!netHandle) {
		//std::cout << __FUNCTION__ << " failed to create network handle!" << std::endl;
		return false;
	}

	return true;
#endif
}
#ifndef ORBISNET
bool GameServer::SendGlobalPacket(int msgID) {
	GamePacket packet;
	packet.type = msgID;

	return SendGlobalPacket(packet);
}

bool GameServer::SendGlobalPacket(GamePacket& packet) {
	ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
	enet_host_broadcast(netHandle, 0, dataPacket); // Sends to every client connected to server
	return true;
}

bool GameServer::SendPacketToPeer(int peerID, int msgID)
{
	GamePacket packet;
	packet.type = msgID;

	ENetPeer* peer = connectedClients.find(peerID)->second;

	return SendPacketToPeer(peer, packet);
}

bool GameServer::SendPacketToPeer(int peerID, GamePacket& packet) {
	ENetPeer* peer = connectedClients.find(peerID)->second;
	return SendPacketToPeer(peer, packet);
}

bool GameServer::SendPacketToPeer(ENetPeer* peer, GamePacket& packet)
{
	ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
	enet_peer_send(peer, 0, dataPacket);
	return true;
}
#endif
void GameServer::UpdateServer() {
#ifndef ORBISNET
	if (!netHandle) {
		return;
	}

	ENetEvent event;
	while (enet_host_service(netHandle, &event, 0) > 0) {
		int type = event.type;
		ENetPeer* p = event.peer;

		int peer = p->incomingPeerID;
		int playerID = peer + 1;
		if (type == ENetEventType::ENET_EVENT_TYPE_CONNECT) {
			AssignIDPacket newID(playerID); // Assign
			SendPacketToPeer(p, newID);
			NewPlayerPacket player(playerID);
			SendGlobalPacket(player);
			connectedClients.insert(std::pair<int, ENetPeer*>(peer, p));
		}
		else if (type == ENetEventType::ENET_EVENT_TYPE_DISCONNECT) {
			PlayerDisconnectPacket player(playerID);
			SendGlobalPacket(player);
			connectedClients.erase(peer);

			NetworkedGame::GetInstance()->RemovePlayerFromServer(playerID);
		}
		else if (type == ENetEventType::ENET_EVENT_TYPE_RECEIVE) {
			GamePacket* packet = (GamePacket*)event.packet->data;
			ProcessPacket(packet, peer);
		}
		enet_packet_destroy(event.packet);
	}
#endif
}

//Second networking tutorial stuff

void GameServer::SetGameWorld(GameWorld& g) {
	gameWorld = &g;
}