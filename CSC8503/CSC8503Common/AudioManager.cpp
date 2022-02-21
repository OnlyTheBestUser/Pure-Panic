#include "AudioManager.h"
#include <iostream>
using namespace NCL;
using namespace NCL::CSC8503;
using namespace FMOD;

AudioManager* AudioManager::inst = NULL;
Studio::System* AudioManager::studioSystem = NULL;
System* AudioManager::system = NULL;
int AudioManager::nextChannelID = 0;

AudioManager::AudioManager() {

}

AudioManager::~AudioManager() {
	Close();
}

void AudioManager::Initialize() {
	studioSystem = NULL;
	IsErroneous(Studio::System::create(&studioSystem));
	studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL);

	system = NULL;
	studioSystem->getCoreSystem(&system);
}

AudioManager* AudioManager::GetInstance()
{
	if (AudioManager::inst == NULL)
		AudioManager::inst = new AudioManager();
	return AudioManager::inst;
}

void AudioManager::Close() {
	studioSystem->unloadAll();
	studioSystem->release();
}

void AudioManager::Update() {
	//TODO erase channels that aren't playing anything anymore & release sounds

	//TODO update 3d listener position
	
	studioSystem->update();
}

void AudioManager::UpdateAudioListener(int listenerID, const Vector3& position, const Quaternion& orientration) {
	Vector3 forwardRot = orientration * Vector3(0, 0, -1);
	Vector3 upRot = orientration * Vector3(0, 1, 0);
	system->set3DListenerAttributes(listenerID, VectorToFMODVector(position), 0,VectorToFMODVector(forwardRot), VectorToFMODVector(upRot));
}

/*Adds the sound into the sound map, if the name isn't already used.*/
void AudioManager::LoadSound(const std::string& soundName, bool threeDimensional, bool looping, bool stream) {

	//Create mode bitmask
	FMOD_MODE mode = FMOD_DEFAULT;
	mode = mode | (threeDimensional ? FMOD_3D : FMOD_2D);
	mode = mode | (looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	mode = mode | (stream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE);

	FMOD::Sound* newSound = nullptr;
	IsErroneous(system->createSound(soundName.c_str(), mode, nullptr, &newSound));
	
	if (newSound) { sounds[soundName] = newSound; };
}

/*Plays sound, returns channel ID.*/
int AudioManager::StartPlayingSound(const std::string& soundName, const Vector3& position, float volumePercent) {
	int newChannel = nextChannelID + 1;
	FMOD::Channel* channel = nullptr;
	Sound* sound = FindSound(soundName);
	if (sound != nullptr) {
		IsErroneous(system->playSound(sounds[soundName], nullptr, true, &channel));
		if (channel) {
			FMOD_MODE mode;
			sound->getMode(&mode);
			//if the 3d flag is set, update positional audio
			if (mode & FMOD_3D) {
				FMOD_VECTOR posF = VectorToFMODVector(position);
				channel->set3DAttributes(&posF, nullptr);
			}
			channel->setVolume(volumePercent);
			channel->setPaused(false);
			channels.insert(std::pair<int, Channel*>(nextChannelID, channel));
		}
		nextChannelID++;
		return newChannel;
	}
	std::cout << "FMOD Sound hasn't been loaded - " << soundName << std::endl;
	return -1;
}

AudioManager& AudioManager::SetChannelVolume(int channelID, const float& DBvol) {
	Channel* channel = FindChannel(channelID);
	if (channel != nullptr) {
		channel->setVolume(DBvol);
	}
	return *inst;
}
AudioManager& AudioManager::SetChannelPitch(int channelID, const float& pitch){
	Channel* channel = FindChannel(channelID);
	if (channel != nullptr) {
		channel->setPitch(pitch);
	}
	return *inst;
}

AudioManager& AudioManager::SetChannel3DPos(int channelID, const Vector3& position) {
	Channel* channel = FindChannel(channelID);
	if (channel != nullptr) {
		FMOD_VECTOR Fpos = VectorToFMODVector(position);
		channel->set3DAttributes(&Fpos, nullptr);
	}
	return *inst;
}

Channel* AudioManager::FindChannel(int channelID) {
	auto i = channels.find(channelID);
	return (i == channels.end() ? nullptr : (*i).second);
}
Sound* AudioManager::FindSound(std::string soundName) {
	auto i = sounds.find(soundName);
	return (i == sounds.end() ? nullptr : (*i).second);
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

bool AudioManager::IsErroneous(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		std::cout << "FMOD Error No: " << result << std::endl;
		return true;
	}
	return true;
}
