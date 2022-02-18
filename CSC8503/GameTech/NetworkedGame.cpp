#include "NetworkedGame.h"
#include "NetworkedPlayer.h"
#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

#define COLLISION_MSG 30

struct MessagePacket : public GamePacket {
	short playerID;
	short messageID;

	MessagePacket() {
		type = Message;
		size = sizeof(short) * 2;
	}
};

NetworkedGame::NetworkedGame() {
	thisServer = nullptr;
	thisClient = nullptr;

	NetworkBase::Initialise();
	timeToNextPacket = 0.0f;
	packetsToSnapshot = 0;

	//for (int i = 0; i < 4; i++) {
	//	networkObjects.push_back(0);
	//}
	//networkObjects.resize(4);
}

NetworkedGame::~NetworkedGame() {
	delete thisServer;
	delete thisClient;
}

void NetworkedGame::StartAsServer() {
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);

	thisServer->RegisterPacketHandler(Received_State, this);
	thisServer->RegisterPacketHandler(Ack, this);
	thisServer->SendGlobalPacket(StringPacket("Server says hello!"));

	SpawnPlayer();
	StartLevel();
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	thisClient = new GameClient();
	thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());

	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	thisClient->RegisterPacketHandler(Assign_ID, this);
	//thisClient->RegisterPacketHandler(String_Message, this);

	// Add one for the server
	GameObject* serverPlayer = AddSphereToWorld(Vector3(5, 1, 5), 1);
	serverPlayer->SetNetworkObject(new NetworkObject(*serverPlayer, 0));
	if (networkObjects.size() <= 0) {
		networkObjects.resize(1); // 4 max players
	}
	networkObjects[0] = (serverPlayer->GetNetworkObject());
}

void NetworkedGame::UpdateGame(float dt) {
	timeToNextPacket -= dt;
	if (timeToNextPacket < 0) {
		if (thisServer) {
			UpdateAsServer(dt);
		}
		else if (thisClient) {
			UpdateAsClient(dt);
		}
		timeToNextPacket += 1.0f / 60.0f; //60hz server/client update
	}

	if (!thisServer && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		StartAsServer();
		std::cout << "Server start" << std::endl;
	}
	if (!thisClient && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		StartAsClient(127, 0, 0, 1);
		std::cout << "Client start" << std::endl;
	}

	TutorialGame::UpdateGame(dt);
}

void NetworkedGame::UpdateAsServer(float dt) {
	thisServer->UpdateServer();
	packetsToSnapshot--;
	if (packetsToSnapshot < 0) {
		BroadcastSnapshot(false);
		packetsToSnapshot = 5;
	}
	//else {
	//	BroadcastSnapshot(true);
	//}
}


// TODO reserve 65k, get guard clause to check for null instead
void NetworkedGame::UpdateAsClient(float dt) {
	thisClient->UpdateClient();

	ClientPacket newPacket;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		//fire button pressed!
		std::cout << "Fire!" << std::endl;
		newPacket.clientID = playerID;
		newPacket.buttonstates[0] = 1;
		clientLastPacketID++;
		newPacket.lastID = clientLastPacketID; //You'll need to work this out somehow...
		thisClient->SendPacket(newPacket);
	}
}

// what is happening here then?? Why not just have a list of network objects??
void NetworkedGame::BroadcastSnapshot(bool deltaFrame) {

	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		//TODO - you'll need some way of determining
		//when a player has sent the server an acknowledgement
		//and store the lastID somewhere. A map between player
		//and an int could work, or it could be part of a 
		//NetworkPlayer struct.
		//auto it = ackHistory.find(clientID);
		//if (it == ackHistory.end())
		//	continue;

		//int playerState = it->second; // make map of clientID -> Last ACK

		// TODO For the time being, just send full packets.

		int playerState = 0;
		GamePacket* newPacket = nullptr;
		if (o->WritePacket(&newPacket, deltaFrame, playerState)) {
			thisServer->SendGlobalPacket(*newPacket);
			delete newPacket;
		}
	}
}

void NetworkedGame::UpdateMinimumState() {
	//Periodically remove old data from the server
	int minID = INT_MAX;
	int maxID = 0; //we could use this to see if a player is lagging behind?

	for (auto i : stateIDs) {
		minID = min(minID, i.second);
		maxID = max(maxID, i.second);
	}
	//every client has acknowledged reaching at least state minID
	//so we can get rid of any old states!
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		o->UpdateStateHistory(minID); //clear out old states so they arent taking up memory...
	}
}




void NetworkedGame::SpawnPlayer() {
	localPlayer = AddCubeToWorld(Vector3(0, 0, 0), Vector3(1, 1, 1), 10.0f);
	localPlayer->SetNetworkObject(new NetworkObject(*localPlayer, playerID));
	std::cout << "Player Spawned with Network ID: " << localPlayer->GetNetworkObject()->GetNetID() << "." << std::endl;
	if (!(playerID < networkObjects.size())) {
		networkObjects.resize(playerID + 1);
	}
	networkObjects[playerID] = (localPlayer->GetNetworkObject());
	for (int i = 0; i < playerID; i++) {
		if (!networkObjects[i]) {
			GameObject* p = AddSphereToWorld(Vector3(10, 10, 10), 1);
			p->SetNetworkObject(new NetworkObject(*p, i));
			networkObjects[i] = p->GetNetworkObject();
		}
	}
}

void NetworkedGame::StartLevel() {

}

void NetworkedGame::ReceivePacket(int type, GamePacket* payload, int source) {
	//SERVER version of the game will receive these from the clients
	if (type == Received_State) {
		ClientPacket* realPacket = (ClientPacket*)payload;
		HandleClientPacket(realPacket);
	}
	else if (type == Ack) {
		AckPacket* realPacket = (AckPacket*)payload;
		std::cout << "Client ID: " << realPacket->clientID << "\nAck: " << realPacket->ack << "\n";
		auto it = ackHistory.find(realPacket->clientID);
		if (it == ackHistory.end()) {
			std::cout << "Not here!!!" << std::endl;
		}
		else {
			if (it->second < realPacket->ack) {
				it->second = realPacket->ack;
				std::cout << "Ack updated" << std::endl;
			}
		}
	}
	//CLIENT version of the game will receive these from the servers
	else if (type == Delta_State) {
		//std::cout << "Client: Received delta message!" << std::endl;
		DeltaPacket* realPacket = (DeltaPacket*)payload;
		if (realPacket->objectID < (int)networkObjects.size()) {
			networkObjects[realPacket->objectID]->ReadPacket(*realPacket);
		}
	}
	else if (type == Full_State) {
		//std::cout << "Client: Received full_state message!" << std::endl;
		FullPacket* realPacket = (FullPacket*)payload;
		//std::cout << realPacket->objectID << ": (" << realPacket->fullState.position.x << ", " << realPacket->fullState.position.y << ", " << realPacket->fullState.position.z << ")\n";
		if (realPacket->objectID < (int)networkObjects.size()) {
			if (networkObjects[realPacket->objectID])
				networkObjects[realPacket->objectID]->ReadPacket(*realPacket);
			else
				std::cout << "Doesn't exist!" << std::endl;
		}
	}
	else if (type == Message) {
		MessagePacket* realPacket = (MessagePacket*)payload;

		if (realPacket->messageID == COLLISION_MSG) {
			std::cout << "Client: Received collision message!" << std::endl;
		}
	}
	else if (type == Assign_ID) {
		AssignIDPacket* realPacket = (AssignIDPacket*)payload;
		std::cout << "ID Assigned: " << realPacket->playerID << std::endl;
		playerID = realPacket->playerID;
		SpawnPlayer();
	}
	else if (type == Player_Connected) {
		NewPlayerPacket* realPacket = (NewPlayerPacket*)payload;
		std::cout << "Client: New player connected!" << std::endl;
		std::cout << "_Player ID: " << realPacket->playerID << std::endl;
		//playerID = realPacket->playerID;
		// Broadcast to everyone

		if (realPacket->playerID != playerID) {
			GameObject* newPlayer = AddSphereToWorld(Vector3(10, 0, 10), 1, 10.0f);
			newPlayer->SetNetworkObject(new NetworkObject(*newPlayer, realPacket->playerID));
			std::cout << "Player Spawned with Network ID: " << newPlayer->GetNetworkObject()->GetNetID() << "." << std::endl;
			if (!(realPacket->playerID < networkObjects.size())) {
				networkObjects.resize(realPacket->playerID + 1);
			}
			networkObjects[newPlayer->GetNetworkObject()->GetNetID()] = (newPlayer->GetNetworkObject());
		}
	}
	else if (type == Player_Disconnected) {
		PlayerDisconnectPacket* realPacket = (PlayerDisconnectPacket*)payload;
		std::cout << "Client: Player Disconnected!" << std::endl;
	}
}

void NetworkedGame::OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b) {
	if (thisServer) { //detected a collision between players!
		MessagePacket newPacket;
		newPacket.messageID = COLLISION_MSG;
		newPacket.playerID = a->GetPlayerNum();

		thisClient->SendPacket(newPacket);

		newPacket.playerID = b->GetPlayerNum();
		thisClient->SendPacket(newPacket);
	}
}

void NetworkedGame::HandleClientPacket(ClientPacket* packet)
{
	auto index = clientHistory.find(packet->clientID);

	if (index != clientHistory.end()) {
		if (index->second < packet->lastID) {
			index->second = packet->lastID;
			auto obj = serverPlayers.find(packet->clientID);
			obj->second->GetTransform().SetPosition(obj->second->GetTransform().GetPosition() + Vector3(0, 5, 0));
		}
	}
	else {
		AddNewPlayerToServer(packet->clientID, packet->lastID);
	}
}

void NetworkedGame::AddNewPlayerToServer(int clientID, int lastID)
{
	clientHistory.insert(std::pair<int, int>(clientID, lastID));

	GameObject* client = AddSphereToWorld(Vector3(), 1.0f, 10.0f);
	client->SetNetworkObject(new NetworkObject(*client, clientID));

	serverPlayers.insert(std::pair<int, GameObject*>(clientID, client));

	if (!(clientID < networkObjects.size()))
		networkObjects.resize(clientID + 1);
	networkObjects[clientID] = client->GetNetworkObject();
	std::cout << "New player added to server: ClientID (" << clientID << "), LastID(" << lastID << ")\n";
}