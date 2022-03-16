#include "NetworkedPlayer.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

NetworkPlayer::NetworkPlayer(NetworkedGame* game, int num) {
	this->game = game;
	playerNum = num;
}

NetworkPlayer::~NetworkPlayer() {

}

void NetworkPlayer::OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) {
	if (game) {
		if (dynamic_cast<NetworkPlayer*>(otherObject))
		{
			game->OnPlayerCollision(this, (NetworkPlayer*)otherObject);
		}
	}
}