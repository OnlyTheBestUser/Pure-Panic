#include "NetworkBase.h"
#ifndef ORBISNET
#include <enet/enet.h>
#endif
#include <iostream>

NetworkBase::NetworkBase() {
#ifndef ORBISNET
	netHandle = nullptr;
#endif
}

NetworkBase::~NetworkBase() {
#ifndef ORBISNET
	if (netHandle) {
		enet_host_destroy(netHandle);
	}
#endif
}

void NetworkBase::Initialise() {
#ifndef ORBISNET
	enet_initialize();
#endif
}

void NetworkBase::Destroy() {
#ifndef ORBISNET
	enet_deinitialize();
#endif
}

bool NetworkBase::ProcessPacket(GamePacket* packet, int peerID) {
	PacketHandlerIterator firstHandler;
	PacketHandlerIterator lastHandler;

	bool canHandle = GetPacketHandlers(packet->type, firstHandler, lastHandler);

	if (canHandle) {
		for (auto i = firstHandler; i != lastHandler; ++i) {
			i->second->ReceivePacket(packet->type, packet, peerID);
		}
		return true;
	}
	std::cout << __FUNCTION__ << " no handler for packet type " << packet->type << std::endl;
	return false;
}