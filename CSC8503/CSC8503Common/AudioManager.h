#pragma once
#include "../../Plugins/FMod/api/studio/inc/fmod_studio.hpp"
#include "../../Plugins/FMod/api/core/inc/fmod.hpp"
#include "../../Common/Vector3.h"
#include <string>
#include <map>

using namespace NCL::Maths;

namespace NCL {
	namespace CSC8503 {
		class AudioManager
		{
			//Singleton AudioManager, should be called by anything that wants
			//to make a sound
		private:
			AudioManager();

			static FMOD::Studio::System* studioSystem;
			static FMOD::System* system;
			static int nextChannelID;

			static std::map<std::string, FMOD::Studio::Bank> banks;
			static std::map<std::string, FMOD::Channel> channels;
			static std::map<std::string, FMOD::Studio::EventInstance> events;
			static std::map<std::string, FMOD::Sound> sounds;

		public:
			static AudioManager& instance() {
				static AudioManager INSTANCE;
				return INSTANCE;
			}

			static void			Initialize();
			static void			Update();
			static void			Close();

			static void				LoadSoundBank(const std::string& bankName);
			static void				LoadSound(const std::string& soundName, bool threeDimensional = false, bool looping = false, bool stream = false);
			static void				UnLoadSound(const std::string& soundName);
			static int				StartPlayingSound(const std::string& soundName, const Vector3& position = Vector3(0, 0, 0), float volumePercent = 1);
			static AudioManager&	SetChannelVolume(int channelID, const float& DBvol);
			static AudioManager&	SetChannelPitch(int channelID, const float& pitch);
			static AudioManager&	SetChannelPosition(int channelID, const Vector3& pos);
			static AudioManager&	SetChannelReverb(int channelID, const FMOD_REVERB3D& reverbProperties);
			static AudioManager&	SetChannelProperties(int channelID, const float& vol, const float& pitch, const Vector3& pos, const FMOD_REVERB3D& reverbProperties);

			static float		DBToLinearVol(float dbVol);
			static float		LinearVolToDB(float linearVol);
			static FMOD_VECTOR	VectorToFMODVector(const Vector3& v);
		};
	}
}

