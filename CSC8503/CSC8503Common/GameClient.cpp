#include "GameClient.h"
#include <iostream>
#include <string>

using namespace NCL;
using namespace CSC8503;

GameClient::GameClient() {
#ifndef ORBISNET
	netHandle = enet_host_create(nullptr, 1, 1, 0, 0);
#endif
}

GameClient::~GameClient() {
	//threadAlive = false;
	//updateThread.join();
#ifndef ORBISNET
	enet_host_destroy(netHandle);
#endif
}

bool GameClient::Connect(uint8_t a, uint8_t b, uint8_t c, uint8_t d, int portNum) {
#ifndef ORBISNET
	ENetAddress address;
	address.port = portNum;

	address.host = (d << 24) | (c << 16) | (b << 8) | (a);

	netPeer = enet_host_connect(netHandle, &address, 2, 0);

	if (netPeer != nullptr) {
		//threadAlive = true;
		//updateThread = std::thread(&GameClient::ThreadedUpdate, this);
	}

	return netPeer != nullptr;
#else
	return false;
#endif
}

void GameClient::UpdateClient() {
#ifndef ORBISNET
	if (netHandle == nullptr)
	{
		return;
	}
#else
	return;
#endif
	//Handle all incoming packets & send any packets awaiting dispatch
#ifndef ORBISNET
	ENetEvent event;
	while (enet_host_service(netHandle, &event, 0) > 0)
	{
		if (event.type == ENET_EVENT_TYPE_CONNECT) {
			std::cout << "Client: Connected to server!" << std::endl;
		}
		else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
			//std::cout << "Client: Packet recieved..." << std::endl;
			GamePacket* packet = (GamePacket*)event.packet->data;
			ProcessPacket(packet);
		}
		enet_packet_destroy(event.packet);
	}
#endif
}

void GameClient::SendPacket(GamePacket& payload) {
#ifndef ORBISNET
	ENetPacket* dataPacket = enet_packet_create(&payload, payload.GetTotalSize(), 0);

	int test = enet_peer_send(netPeer, 0, dataPacket);
#endif
}

//void GameClient::ThreadedUpdate() {
//	while (threadAlive) {
//		UpdateClient();
//	}
//}
