#include "NetworkedGame.h"
#include "NetworkedPlayer.h"
#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"
#include "../GameTech/LevelLoader.h"

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
	GameObject* serverPlayer = levelLoader->AddDummyPlayerToWorld(Vector3(0, 10, 0));
	serverPlayer->SetNetworkObject(new NetworkObject(*serverPlayer, 0));
	serverPlayer->SetDynamic(true);
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
	//packetsToSnapshot--;
	//if (packetsToSnapshot < 0) {
	//	BroadcastSnapshot(false);
	//	packetsToSnapshot = 5;
	//}
	//else {
	//	BroadcastSnapshot(true);
	//}

	BroadcastSnapshot(false);
}

void NetworkedGame::UpdateAsClient(float dt) {
	thisClient->UpdateClient();

	if (!localPlayer)
		return;

	ClientPacket newPacket;
	newPacket.clientID = playerID;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		//fire button pressed!
		std::cout << "Fire!" << std::endl;
		newPacket.buttonstates[0] = 1;
	}
	clientLastPacketID++;
	newPacket.lastID = clientLastPacketID;
	Vector3 position = localPlayer->GetTransform().GetPosition();
	newPacket.pos[0] = position.x;
	newPacket.pos[1] = position.y;
	newPacket.pos[2] = position.z;

	Vector3 angle = localPlayer->GetTransform().GetOrientation().ToEuler();
	newPacket.angles[0] = angle.x;
	newPacket.angles[1] = angle.y;
	newPacket.angles[2] = angle.z;
	thisClient->SendPacket(newPacket);
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
	localPlayer = player1;
	localPlayer->SetNetworkObject(new NetworkObject(*localPlayer, playerID));
	localPlayer->SetDynamic(true);
	localPlayer->GetTransform().SetPosition(Vector3(playerID * 5, 10, 0));
	std::cout << "Player Spawned with Network ID: " << localPlayer->GetNetworkObject()->GetNetID() << "." << std::endl;
	if (!(playerID < networkObjects.size())) {
		networkObjects.resize(playerID + 1);
	}
	networkObjects[playerID] = (localPlayer->GetNetworkObject());
	for (int i = 0; i < playerID; i++) {
		if (!networkObjects[i]) {
			GameObject* p = levelLoader->AddDummyPlayerToWorld(Vector3(i * 5, 10, 0));
			p->SetDynamic(true);
			p->GetPhysicsObject()->SetGravity(false);
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
		if (realPacket->objectID == playerID)
			return;
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
			GameObject* newPlayer = levelLoader->AddDummyPlayerToWorld(Vector3(10, 15, 10));
			newPlayer->SetNetworkObject(new NetworkObject(*newPlayer, realPacket->playerID));
			newPlayer->SetDynamic(true);
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
			Vector3 pos(packet->pos[0], packet->pos[1], packet->pos[2]);
			obj->second->GetTransform().SetPosition(pos);
			obj->second->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(packet->angles[0], packet->angles[1], packet->angles[2]));
		}
	}
	else {
		AddNewPlayerToServer(packet->clientID, packet->lastID);
	}
}

void NetworkedGame::AddNewPlayerToServer(int clientID, int lastID)
{
	clientHistory.insert(std::pair<int, int>(clientID, lastID));

	GameObject* client = levelLoader->AddDummyPlayerToWorld(Vector3(clientID * 5, 10, 0));
	client->SetNetworkObject(new NetworkObject(*client, clientID));
	client->SetDynamic(true);
	client->GetPhysicsObject()->SetGravity(false);

	serverPlayers.insert(std::pair<int, GameObject*>(clientID, client));

	if (!(clientID < networkObjects.size()))
		networkObjects.resize(clientID + 1);
	networkObjects[clientID] = client->GetNetworkObject();
	std::cout << "New player added to server: ClientID (" << clientID << "), LastID(" << lastID << ")\n";
}