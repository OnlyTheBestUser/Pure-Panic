#include "BGMManager.h"
#include "AudioManager.h"

#ifndef _ORBIS
using namespace NCL;

BGMManager* BGMManager::inst = nullptr;
int BGMManager::curSongChannel = -1;

BGMManager::BGMManager() {
	audioManager = NCL::AudioManager::GetInstance();
}

BGMManager* BGMManager::GetInstance()
{
	if (BGMManager::inst == nullptr)
		BGMManager::inst = new BGMManager();
	return BGMManager::inst;
}

int BGMManager::PlaySong(std::string songName, float posInSong) {
	StopMusic();
	curSongChannel = audioManager->StartPlayingSound(songName, Vector3(0,0,0), 0.7f);
	return curSongChannel;
}

int BGMManager::PlaySongFade(std::string songName, float fadeInTime, float posInSong) {
	StopMusic();
	curSongChannel = audioManager->FadeInSound(songName, fadeInTime);
	return curSongChannel;
}

void BGMManager::SetVolume(float percentVol) {
	audioManager->SetChannelVolume(curSongChannel, percentVol);
}


void BGMManager::StopMusic() {
	if (curSongChannel != -1) {
		audioManager->StopChannel(curSongChannel);
	}
}
#endif // !_ORBIS