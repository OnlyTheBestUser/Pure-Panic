#pragma once

#ifdef _ORBIS
#include "../../Plugins/FMod/PS4/api/studio/inc/fmod_studio.hpp"
#include "../../Plugins/FMod/PS4/api/studio/inc/fmod_studio_common.h"
#endif

#ifdef _WIN64
#include "../../Plugins/FMod/Windows/api/studio/inc/fmod_studio.hpp"
#include "../../Plugins/FMod/Windows/api/studio/inc/fmod_studio_common.h"
#endif

#include "../../Common/Vector3.h"
#include <windows.h>
#include <string>
#include <map>
#include "../../Common/Quaternion.h"

using namespace NCL::Maths;

namespace NCL {
	namespace CSC8503 {
		class AudioManager
		{
			//Singleton AudioManager, should be called by anything that wants
			//to make a sound
		private:
			AudioManager();
			~AudioManager();

			static AudioManager* inst;
			static FMOD::Studio::System* studioSystem;
			static FMOD::System* system;
			static int nextChannelID;

			std::map<std::string, FMOD::Studio::Bank*> banks;
			std::map<int, FMOD::Channel*> channels;
			std::map<std::string, FMOD::Studio::EventInstance*> events;
			std::map<std::string, FMOD::Sound*> sounds;
			

			bool					IsErroneous(FMOD_RESULT result);
			FMOD::Channel*			FindChannel(int channelID);
			FMOD::Sound*			FindSound(std::string soundName);

		public:
			static AudioManager*	GetInstance();

			void					Initialize();
			void					Update();
			void					Close();

			void					LoadSoundBank(const std::string& bankName);
			void					LoadSound(const std::string& soundName, bool threeDimensional = false, bool looping = false, bool stream = false);
			void					UnLoadSound(const std::string& soundName);
			int						StartPlayingSound(const std::string& soundName, const Vector3& position = Vector3(0, 0, 0), const float& volumePercent = 1.0f, const float& positionInSong = 0.0f);
			int						FadeInSound(const std::string& soundName, float fadeInTime, const Vector3& position = Vector3(0, 0, 0), const float& volumePercent = 1.0f, const float& positionInSong = 0.0f);
			AudioManager&			SetChannelVolume(int channelID, const float& volPercent);
			AudioManager&			SetChannelPitch(int channelID, const float& pitch);
			AudioManager&			SetChannel3DPos(int channelID, const Vector3& position);
			void					StopChannel(int channelID);
			void					StopLoopingSound(int channelID);
			void					UpdateAudioListener(int audioListenerID, const Vector3& position, const Quaternion& orientration);

			float					DBToLinearVol(float dbVol);
			float					LinearVolToDB(float linearVol);
			FMOD_VECTOR*			VectorToFMODVector(const Vector3& v);
		};
	}
}


