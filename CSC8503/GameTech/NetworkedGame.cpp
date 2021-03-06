#include "NetworkedGame.h"
#include "NetworkedPlayer.h"
#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"
#include "../GameTech/LevelLoader.h"
#include "../CSC8503Common/InputHandler.h"

#define COLLISION_MSG 30

using namespace NCL;
using namespace CSC8503;

NetworkedGame* NetworkedGame::singleton = nullptr;

struct MessagePacket : public GamePacket {
	short playerID;
	short messageID;

	MessagePacket() {
		type = Message;
		size = sizeof(short) * 2;
	}
};

NetworkedGame::NetworkedGame(string mapString) : TutorialGame(mapString) {
	levelLoader = new LevelLoader(physics, renderer, this);

	thisServer = nullptr;
	thisClient = nullptr;

	NetworkBase::Initialise();

	timeToNextPacket = 0.0f;
	packetsToSnapshot = 0;

	singleton = this;
	
	emptyPlayer = new GameObject();
	emptyPlayer->SetPhysicsObject(new PhysicsObject(&emptyPlayer->GetTransform(), new CollisionVolume()));

	Command* startServer = new StartServerCommand(this);
	Command* startClient = new StartClientCommand(this);

	inputHandler->BindButton(START_SERVER, startServer);
	inputHandler->BindButton(START_CLIENT, startClient);

	//InitialiseAssets();
}

NetworkedGame::~NetworkedGame() {
	delete thisServer;
	delete thisClient;
	delete emptyPlayer;
}

void NetworkedGame::StartAsServer() {
	if (thisServer || thisClient) {
		return;
	}

	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);

	thisServer->RegisterPacketHandler(Received_State, this);
	thisServer->RegisterPacketHandler(PowerUp_State, this);
	thisServer->RegisterPacketHandler(Death_State, this);

	Command* startGame = new StartGameCommand(this);
	Command* resetGame = new ResetGameCommand(this);

	inputHandler->BindButton(START_GAME, startGame);
	inputHandler->BindButton(RESET_GAME, resetGame);

	SpawnPlayer();

}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	if (thisServer || thisClient) {
		return;
	}

	thisClient = new GameClient();
	thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());

	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Fire_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	thisClient->RegisterPacketHandler(Assign_ID, this);
	thisClient->RegisterPacketHandler(PowerUp_State, this);
	thisClient->RegisterPacketHandler(Death_State, this);
	thisClient->RegisterPacketHandler(Game_State, this);
}

void NetworkedGame::UpdateGame(float dt) {

	if (!thisServer && !thisClient) {
		Debug::Print("Press 1 to Start as Host", Vector2(5, 30), 20.0f, Debug::BLUE);
		Debug::Print("Press 2 to Start as Client", Vector2(5, 45), 20.0f, Debug::BLUE);
	}

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

	for (auto x : powerups) {
		if (x->IsPickedUp()) {
			PowerUpPacket powerUpPacket;
			powerUpPacket.worldID = x->GetWorldID();
			powerUpPacket.clientID = playerID;
			if (thisClient)
				thisClient->SendPacket(powerUpPacket);
#ifndef ORBISNET
			if (thisServer)
				thisServer->SendGlobalPacket(powerUpPacket);
#endif
		}
	}

	TutorialGame::UpdateGame(dt);
}

void NetworkedGame::UpdateAsServer(float dt) {
	thisServer->UpdateServer();

	if (localPlayer->IsFiring()) {
		FirePacket* newPacket = new FirePacket();
		newPacket->clientID = localPlayer->GetNetworkObject()->GetNetID();
		newPacket->pitch = localPlayer->GetCam()->GetPitch();
		newPacket->bulletCounter = localPlayer->BulletCounter;
		newPacket->spread = (localPlayer->GetCurrentPowerup() == PowerUpType::MultiBullet);
    
#ifndef ORBISNET
		thisServer->SendGlobalPacket(*newPacket);
#endif

		delete newPacket;
	}

	if (gameManager->IsTimerFinished()) {
		SendEndGamePacket();
	}
	else {
		SendUpdateGamePacket();
	}

	BroadcastSnapshot();
}

void NetworkedGame::UpdateAsClient(float dt) {

	thisClient->UpdateClient();

	if (!localPlayer)
		return;

	ClientPacket newPacket;
	newPacket.clientID = playerID;
	clientLastPacketID++;
	newPacket.lastID = clientLastPacketID;

	Vector3 position = localPlayer->GetTransform().GetPosition();
	newPacket.pos[0] = position.x;
	newPacket.pos[1] = position.y;
	newPacket.pos[2] = position.z;

	newPacket.yaw = localPlayer->GetTransform().GetOrientation().ToEuler().y;
	newPacket.pitch = localPlayer->GetCam()->GetPitch();

	newPacket.firing = localPlayer->IsFiring();
	newPacket.spread = (localPlayer->GetCurrentPowerup() == PowerUpType::MultiBullet);
	newPacket.bulletCounter = localPlayer->BulletCounter;

	thisClient->SendPacket(newPacket);
}

void NetworkedGame::BroadcastSnapshot() {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; i++) {
		NetworkObject* o = (*i)->GetNetworkObject();

		if (!o || o == nullptr) {
			continue;
		}

		GamePacket* newPacket = nullptr;
		if (o->WritePacket(&newPacket)) {
#ifndef ORBISNET
			thisServer->SendGlobalPacket(*newPacket);
#endif
			delete newPacket;
		}
	}
}

void NetworkedGame::SpawnPlayer() {
	localPlayer = player1;
#ifndef _ORBIS
	localPlayer->SetNetworkObject(new NetworkObject(*localPlayer, playerID));
#endif
	localPlayer->GetPhysicsObject()->SetDynamic(true);

	int index = playerID == 0 ? 0 : playerID % spawnPoints.size();
	
	Vector3 spawnPos = spawnPoints[index];

	localPlayer->GetTransform().SetPosition(spawnPos);
	localPlayer->SetSpawn(spawnPos);
}

void NetworkedGame::StartLevel() {
	renderer->drawGUI = true;
	SetState(GameState::PLAY);
	ResetLevel();
	gameManager->StartRound();
	SendStartGamePacket();
}

void NetworkedGame::ServerResetLevel() {
	ResetLevel();
	SendResetGamePacket();
}

void NetworkedGame::ResetLevel() {
	renderer->ClearPaint();
	gameManager->GetTimer()->ResetTimer();
	gameManager->SetScores(Vector2(0.01f, 0.01f));
	gameManager->printResults = false;
}

void NetworkedGame::ReceivePacket(int type, GamePacket* payload, int source) {
	//SERVER version of the game will receive these from the clients
	if (type == Received_State) {
		ClientPacket* realPacket = (ClientPacket*)payload;
		HandleClientPacket(realPacket);
		return;
	}

	//CLIENT version of the game will receive these from the servers
	// Connection / Initialisation packets
	switch (type) {
		case(Assign_ID):
			HandleAssignID((AssignIDPacket*)payload);
			return;
		case(Player_Connected):
			HandlePlayerConnect((NewPlayerPacket*)payload);
			return;
		case(Player_Disconnected):
			HandlePlayerDisconnect((PlayerDisconnectPacket*)payload);
			return;
		case(Game_State):
			HandleGameState((GameStatePacket*)payload);
			return;
	}

	if (!CheckExists((IDPacket*)payload))
		return;

	switch (type) {
		case(Full_State):
			HandleFullState((FullPacket*)payload);
			return;
		case(Fire_State):
			HandleFireState((FirePacket*)payload);
			return;
		case(Death_State):
			HandleDeathState((DeathPacket*)payload);
			return;
		case(PowerUp_State):
			HandlePowerUp((PowerUpPacket*)payload);
			return;
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
			obj->second->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, packet->yaw, 0));

			if (packet->firing) {
				//std::cout << "Handle Client Packet: " << packet->bulletCounter << std::endl;
				ServerFire(obj->second, packet->pitch, packet->bulletCounter, packet->spread, packet->clientID);
			}
		}
	}
	else {
		AddNewPlayerToServer(packet->clientID, packet->lastID);
	}
}

void NetworkedGame::AddNewPlayerToServer(int clientID, int lastID)
{
	clientHistory.insert(std::pair<int, int>(clientID, lastID));

	GameObject* client = LevelLoader::SpawnDummyPlayer(Vector3(clientID * 5, 10, 0));
	client->SetNetworkObject(new NetworkObject(*client, clientID));
	client->GetPhysicsObject()->SetDynamic(true);
	client->GetPhysicsObject()->SetGravity(false);
	client->GetRenderObject()->SetColour(GameManager::GetColourForID(clientID));

	serverPlayers.insert(std::pair<int, Player*>(clientID, (Player*)client));

	if (!(clientID < networkObjects.size()))
		networkObjects.resize(clientID + 1);
	networkObjects[clientID] = client->GetNetworkObject();
	//std::cout << "New player added to server: ClientID (" << clientID << "), LastID(" << lastID << ")\n";
}

void NetworkedGame::ServerFire(GameObject* owner, float pitch, int bulletCounter, bool spread, int clientID)
{
	Fire(owner, spread, bulletCounter, pitch, clientID);

	//std::cout << "ServerFire: " << bulletCounter << std::endl;

	FirePacket newPacket;
	newPacket.clientID = clientID;
	newPacket.pitch = pitch;
	newPacket.bulletCounter = bulletCounter;
	newPacket.spread = spread;
	
#ifndef ORBISNET
	thisServer->SendGlobalPacket(newPacket); 
#endif
}

void NetworkedGame::SendDeathPacket(int clientID, Vector3 pos)
{
	DeathPacket packet;

	packet.clientID = clientID;
	packet.x = pos.x;
	packet.y = pos.y;
	packet.z = pos.z;

	if (singleton) {
		if (singleton->thisServer) {
#ifndef ORBISNET
			singleton->thisServer->SendGlobalPacket(packet);
#endif
		}
		else if(singleton->thisClient) {
			singleton->thisClient->SendPacket(packet);
		}
	}

}

void NetworkedGame::Fire(GameObject* owner, bool spread, int bulletCounter, float pitch, int clientID)
{
	int num = 1;
	if (spread) {
		num = 5;
	}

	for (int i = 0; i < num; i++) {
		LevelLoader::SpawnProjectile(owner, spread, bulletCounter, false, pitch, clientID);
	}
}

void NetworkedGame::HandleFullState(FullPacket* packet)
{
	if (packet->clientID == playerID)
		return;

	if (packet->clientID < (int) networkObjects.size()) {
		if (networkObjects[packet->clientID])
			networkObjects[packet->clientID]->ReadPacket(*packet);
	}
}

void NetworkedGame::HandleGameState(GameStatePacket* packet) {
	switch (packet->state) {
	case Game_Reset:
		ResetLevel();
		return;
	case Game_Start:
		ResetLevel();
		gameManager->StartRound();
		return;
	case Game_Update:
		gameManager->SetScores(Vector2(packet->team1Score, packet->team2Score));
		return;
	case Game_Over:
		gameManager->printResults = true;
		return;
	}
}

bool NetworkedGame::CheckExists(IDPacket* packet)
{
	if (packet->clientID == playerID)
		return false;
	if (!(packet->clientID < networkObjects.size())) {
		networkObjects.resize(packet->clientID + 1);
	}
	if (!networkObjects[packet->clientID]) {
		GameObject* p = LevelLoader::SpawnDummyPlayer(Vector3(packet->clientID * 5, 10, 0));
		p->GetPhysicsObject()->SetGravity(false);
		p->SetNetworkObject(new NetworkObject(*p, packet->clientID));
		p->GetRenderObject()->SetColour(GameManager::GetColourForID(packet->clientID));
		networkObjects[packet->clientID] = p->GetNetworkObject();
	}
	return true;
}

void NetworkedGame::HandleFireState(FirePacket* packet)
{
	if (packet->clientID == playerID)
		return;
	auto obj = networkObjects[packet->clientID];
	if (obj) {
		Fire(&obj->object, packet->spread, packet->bulletCounter, packet->pitch, packet->clientID);
	}
}

void NetworkedGame::HandleDeathState(DeathPacket* packet)
{
	if (thisServer) {
#ifndef ORBISNET
		thisServer->SendGlobalPacket(*packet);
#endif
	}

	emptyPlayer->GetTransform().SetPosition(Vector3(packet->x, packet->y, packet->z));

	for (int i = 0; i < 10; ++i)
	{
		LevelLoader::SpawnProjectile(emptyPlayer, true, i, false, 90, packet->clientID + 1, 10);
	}
}

void NetworkedGame::HandleAssignID(AssignIDPacket* packet)
{
	//std::cout << "ID Assigned: " << packet->clientID << std::endl;
	playerID = packet->clientID;

	SpawnPlayer();
	localPlayer->SetPlayerID(playerID);
	renderer->playerColour = GameManager::GetColourForID(playerID);
}

void NetworkedGame::HandlePlayerConnect(NewPlayerPacket* packet)
{
	//std::cout << "Client: New player connected!" << std::endl;
	//std::cout << "_Player ID: " << packet->clientID << std::endl;

	if (packet->clientID != playerID) {
		GameObject* newPlayer = LevelLoader::SpawnDummyPlayer(Vector3(10, 15, 10));
		newPlayer->SetNetworkObject(new NetworkObject(*newPlayer, packet->clientID));
		newPlayer->GetPhysicsObject()->SetDynamic(true);
		//std::cout << "Player Spawned with Network ID: " << newPlayer->GetNetworkObject()->GetNetID() << "." << std::endl;
		if (!(packet->clientID < networkObjects.size())) {
			networkObjects.resize(packet->clientID + 1);
		}
		networkObjects[newPlayer->GetNetworkObject()->GetNetID()] = (newPlayer->GetNetworkObject());
		newPlayer->GetRenderObject()->SetColour(GameManager::GetColourForID(packet->clientID));
	}
}

void NetworkedGame::HandlePlayerDisconnect(PlayerDisconnectPacket* packet) {
	RemovePlayerFromServer(packet->clientID);
}

void NetworkedGame::HandlePowerUp(PowerUpPacket* packet)
{
	for (PowerUp* x : powerups) {
		if (x->GetWorldID() == packet->worldID) {
			x->PickUp();
		}
	}

	if (thisServer) {
#ifndef ORBISNET
		thisServer->SendGlobalPacket(*(GamePacket*)packet);
#endif
	}
}

void NetworkedGame::RemovePlayerFromServer(int clientID) {
	delete networkObjects[clientID];
	networkObjects[clientID] = nullptr;

	auto index = clientHistory.find(clientID);
	if (index != clientHistory.end()) {
		clientHistory.erase(index);
	}

	auto sInd = serverPlayers.find(clientID);
	if (sInd != serverPlayers.end()) {
		serverPlayers.erase(sInd);
	}
}

void NetworkedGame::SendResetGamePacket() {
	GameStatePacket newPacket;

	newPacket.state = Game_Reset;
	newPacket.team1Score = 0.1f;
	newPacket.team2Score = 0.1f;

#ifndef ORBISNET
	thisServer->SendGlobalPacket(newPacket);
#endif
	ResetLevel();
}

void NetworkedGame::SendStartGamePacket() {
	GameStatePacket newPacket;

	newPacket.state = Game_Start;
	newPacket.team1Score = 0.1f;
	newPacket.team2Score = 0.1f;

#ifndef ORBISNET
	thisServer->SendGlobalPacket(newPacket);
#endif
}

void NetworkedGame::SendUpdateGamePacket() {
	GameStatePacket newPacket;

	newPacket.state = Game_Update;

	Vector2 scores = gameManager->GetScores();
	newPacket.team1Score = scores.x;
	newPacket.team2Score = scores.y;

#ifndef ORBISNET
	thisServer->SendGlobalPacket(newPacket);
#endif
}

void NetworkedGame::SendEndGamePacket() {
	GameStatePacket newPacket;

	newPacket.state = Game_Over;

	Vector2 scores = gameManager->GetScores();
	newPacket.team1Score = scores.x;
	newPacket.team2Score = scores.y;
#ifndef ORBISNET
	thisServer->SendGlobalPacket(newPacket);
#endif
	gameManager->printResults = true;
}
void NetworkedGame::UpdatePauseState(float dt)
{
	UpdatePauseScreen(dt);
	physics->Update(dt);
	world->UpdateWorld(dt);
	gameManager->Update(dt);
	UpdateScores(dt);
}

void NetworkedGame::UpdatePauseScreen(float dt)
{
	renderer->DrawString("PAUSED", Vector2(5, 80), Debug::MAGENTA, 30.0f);
	renderer->DrawString("Press P to Unpause.", Vector2(5, 90), Debug::WHITE, 20.0f);
	renderer->DrawString("Press Esc to disconnect.", Vector2(5, 95), Debug::WHITE, 20.0f);
}
