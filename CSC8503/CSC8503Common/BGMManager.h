#pragma once
#include <string>
namespace NCL {
	class AudioManager;
	class BGMManager {
	public:
		int PlaySong(std::string songName, float posInSong = 0.0f);
		int PlaySongFade(std::string songName, float fadeInTime, float posInSong = 0.0f);
		void StopMusic();
		void SetVolume(float percentVol);
		static BGMManager* GetInstance();
	private:
		static BGMManager* inst;
		BGMManager();
		NCL::AudioManager* audioManager;
		static int curSongChannel;
	};
}