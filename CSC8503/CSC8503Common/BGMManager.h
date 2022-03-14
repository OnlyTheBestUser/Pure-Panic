#pragma once
#include <string>
namespace NCL {
	class AudioManager;
	namespace CSC8503 {
		class BGMManager {
		public:
			BGMManager(NCL::AudioManager* am) : audioManager(am) {};
			int PlaySong(std::string songName, float posInSong = 0.0f);
			int PlaySongFade(std::string songName, float fadeInTime, float posInSong = 0.0f);
			void StopMusic();
		private:
			NCL::AudioManager* audioManager;
			int curSongChannel = -1;
		};
	}
}