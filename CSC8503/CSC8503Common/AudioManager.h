#pragma once
#include "../../Plugins/FMod/api/studio/inc/fmod_studio.hpp"
#include "../../Plugins/FMod/api/studio/inc/fmod_studio_common.h"
#include "../../Common/Vector3.h"
#include <windows.h>
#include <string>
#include <map>
#include "../../Common/Quaternion.h"

#define AUDIO_PATH std::string("../../Assets/Audio/")

using namespace NCL::Maths;

namespace NCL {
	namespace CSC8503 {
		class AudioManager
		{
			//Singleton AudioManager, should be called by anything that wants
			//to make a sound
			// TODO: Integration with the PS4 version of FMOD
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
			int						StartPlayingSound(const std::string& soundName, const Vector3& position = Vector3(0, 0, 0), float volumePercent = 1);
			AudioManager&			SetChannelVolume(int channelID, const float& DBvol);
			AudioManager&			SetChannelPitch(int channelID, const float& pitch);
			AudioManager&			SetChannel3DPos(int channelID, const Vector3& position);
			void					UpdateAudioListener(int audioListenerID, const Vector3& position, const Quaternion& orientration);

			float					DBToLinearVol(float dbVol);
			float					LinearVolToDB(float linearVol);
			FMOD_VECTOR				VectorToFMODVector(const Vector3& v);
		};
	}
}


