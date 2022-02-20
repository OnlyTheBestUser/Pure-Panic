#pragma once
#include "../../Plugins/FMod/api/studio/inc/fmod_studio.hpp"
#include "../../Plugins/FMod/api/studio/inc/fmod_studio_common.h"
#include "../../Common/Vector3.h"
#include <windows.h>
#include <string>
#include <map>

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
			static AudioManager* inst;

			static FMOD::Studio::System* studioSystem;
			static FMOD::System* system;
			static int nextChannelID;

			std::map<std::string, FMOD::Studio::Bank*> banks;
			std::map<std::string, FMOD::Channel*> channels;
			std::map<std::string, FMOD::Studio::EventInstance*> events;
			std::map<std::string, FMOD::Sound*> sounds;

		public:
			static AudioManager* GetInstance();

			void			Initialize();
			void			Update();
			void			Close();

			//static void				LoadSoundBank(const std::string& bankName);
			void				LoadSound(const std::string& soundName, bool threeDimensional = false, bool looping = false, bool stream = false);
			//static void				UnLoadSound(const std::string& soundName);
			int				StartPlayingSound(const std::string& soundName, const Vector3& position = Vector3(0, 0, 0), float volumePercent = 1);
			//static AudioManager&	SetChannelVolume(int channelID, const float& DBvol);
			//static AudioManager&	SetChannelPitch(int channelID, const float& pitch);
			//static AudioManager&	SetChannelPosition(int channelID, const Vector3& pos);
			//static AudioManager&	SetChannelReverb(int channelID, const FMOD_REVERB3D& reverbProperties);
			//static AudioManager&	SetChannelProperties(int channelID, const float& vol, const float& pitch, const Vector3& pos, const FMOD_REVERB3D& reverbProperties);

			float		DBToLinearVol(float dbVol);
			float		LinearVolToDB(float linearVol);
			FMOD_VECTOR	VectorToFMODVector(const Vector3& v);
			bool IsErroneous(FMOD_RESULT result);
		};
	}
}


