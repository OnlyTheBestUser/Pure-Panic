#include "BGMManager.h"
#include "AudioManager.h"

using namespace NCL;
using namespace CSC8503;

int BGMManager::PlaySong(std::string songName, float posInSong) {
	StopMusic();
	curSongChannel = audioManager->StartPlayingSound(songName);
	return curSongChannel;
}

int BGMManager::PlaySongFade(std::string songName, float fadeInTime, float posInSong) {
	StopMusic();
	curSongChannel = audioManager->FadeInSound(songName, fadeInTime);
	return curSongChannel;
}

void BGMManager::StopMusic() {
	if (curSongChannel != -1) {
		audioManager->StopChannel(curSongChannel);
	}
}