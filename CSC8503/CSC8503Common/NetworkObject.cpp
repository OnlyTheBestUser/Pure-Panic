#include "NetworkObject.h"

using namespace NCL;
using namespace CSC8503;

NetworkObject::NetworkObject(GameObject& o, int id, GameWorld* world) : object(o), world(world), networkID(id) {
	deltaErrors = 0;
	fullErrors = 0;
}

bool NetworkObject::ReadPacket(GamePacket& p) {
	if (p.type == Full_State) {
		return ReadFullPacket((FullPacket&)p);
	}
	return false; //this isn't a packet we care about!
}
bool NetworkObject::WritePacket(GamePacket** p)
{
	return WriteFullPacket(p);
}
//Client objects recieve these packets

bool NetworkObject::ReadFullPacket(FullPacket& p) {
	if (p.fullState.stateID < lastFullState.stateID) {
		return false; // received an 'old' packet, ignore!
	}
	lastFullState = p.fullState;

	//std::cout << "FULL PACKET!" << std::endl;

	object.GetTransform()
		.SetPosition(lastFullState.position)
		.SetOrientation(lastFullState.orientation);

	stateHistory.emplace_back(lastFullState);

	return true;
}

bool NetworkObject::WriteFullPacket(GamePacket** p) {
	FullPacket* fp = new FullPacket();

	fp->clientID = networkID;
	fp->fullState.position = object.GetTransform().GetPosition();
	fp->fullState.orientation = object.GetTransform().GetOrientation();
	fp->fullState.stateID = lastFullState.stateID++;

	*p = fp;
	return true;
}

NetworkState& NetworkObject::GetLatestNetworkState() {
	return lastFullState;
}

bool NetworkObject::GetNetworkState(int stateID, NetworkState& state) {
	for (auto i = stateHistory.begin(); i < stateHistory.end(); ++i) {
		if ((*i).stateID == stateID) {
			state = (*i);
			return true;
		}
	}
	return false;
}

void NetworkObject::UpdateStateHistory(int minID) {
	for (auto i = stateHistory.begin(); i < stateHistory.end(); ) {
		if ((*i).stateID < minID) {
			i = stateHistory.erase(i);
		}
		else {
			++i;
		}
	}
}