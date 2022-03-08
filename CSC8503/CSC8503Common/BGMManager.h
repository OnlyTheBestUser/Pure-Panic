#pragma once
#include <string>
namespace NCL {
	namespace CSC8503 {
		class AudioManager;
		class BGMManager {
		public:
			BGMManager(AudioManager* am) : audioManager(am) {};
			int PlaySong(std::string songName, float posInSong = 0.0f);
			int PlaySongFade(std::string songName, float fadeInTime, float posInSong = 0.0f);
			void StopMusic();
		private:
			AudioManager* audioManager;
			int curSongChannel = -1;
		};
	}
}