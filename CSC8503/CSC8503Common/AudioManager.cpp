#include "AudioManager.h"
using namespace NCL;
using namespace NCL::CSC8503;
using namespace FMOD;

AudioManager::AudioManager() {
}

void AudioManager::Initialize() {
	studioSystem = NULL;
	FMOD::Studio::System::create(&studioSystem);
	studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL);

	system = NULL;
	studioSystem->getCoreSystem(&system);
}

void AudioManager::Close() {
	studioSystem->unloadAll();
	studioSystem->release();
}

void AudioManager::Update() {
	//@TODO erase channels that aren't playing anything anymore
	studioSystem->update();
}

/*Adds the sound into the sound map, if the name isn't already used.*/
void AudioManager::LoadSound(const std::string& soundName, bool threeDimensional, bool looping, bool stream) {
	if (sounds.find(soundName) != sounds.end()) {
		return;
	}

	FMOD_MODE mode = FMOD_DEFAULT;
	mode = mode | (threeDimensional ? FMOD_3D : FMOD_2D);
	mode = mode | (looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	mode = mode | (stream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE);

	FMOD::Sound* newSound = nullptr;
	system->createSound(soundName.c_str(), mode, nullptr, &newSound);
	
	if (newSound) { sounds[soundName] = *newSound; };
}

/*Plays sound, returns channel ID.*/
int AudioManager::StartPlayingSound(const std::string& soundName, const Vector3& position, float volumePercent) {
	int newChannel = nextChannelID++;
	FMOD::Channel* channel = nullptr;
	system->playSound(&sounds[soundName], nullptr, true, &channel);
	if (channel) {
		FMOD_MODE mode;
		sounds[soundName].getMode(&mode);
		//if the 3d flag is set, update positional audio
		if (mode & FMOD_3D) {
			FMOD_VECTOR posF = VectorToFMODVector(position);
			channel->set3DAttributes(&posF, nullptr);
		}
		channel->setVolume(volumePercent);
		channel->setPaused(false);
		SetChannelPitch(1, 1.0f).SetChannelPosition(1, Vector3(1, 1, 1));
	}
	return newChannel;
}

/*Approx DB to Volume Percentage.*/
float AudioManager::DBToLinearVol(float DBvol) {
	return pow(2, DBvol / 6.02);
}

/*Approx Volume Percentage to DB.*/
float AudioManager::LinearVolToDB(float LinearVol) {
	return (6.02 * log2(LinearVol));
}

FMOD_VECTOR	AudioManager::VectorToFMODVector(const Vector3& v) {
	return FMOD_VECTOR{ v.x, v.y, v.z };
}
