#include "BGMManager.h"
#include "AudioManager.h"

using namespace NCL;
using namespace CSC8503;

int BGMManager::PlaySong(std::string songName, float posInSong) {
	curSongChannel = audioManager->StartPlayingSound(songName);
	return curSongChannel;
}

int BGMManager::PlaySongFade(std::string songName, float fadeInTime, float posInSong) {
	curSongChannel = audioManager->StartPlayingSound(songName);
	return curSongChannel;
}

void BGMManager::StopMusic() {
	if (curSongChannel) {
		//stop playing that channel
	}
}