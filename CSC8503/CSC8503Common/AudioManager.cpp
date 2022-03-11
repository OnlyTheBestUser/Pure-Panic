#include "AudioManager.h"
#include <iostream>
#ifndef _ORBIS

using namespace NCL;
using namespace FMOD;

AudioManager* AudioManager::inst = nullptr;
Studio::System* AudioManager::studioSystem = nullptr;
System* AudioManager::system = nullptr;
int AudioManager::nextChannelID = 0;

AudioManager::AudioManager() {

}

AudioManager::~AudioManager() {
	Close();
}

void AudioManager::Initialize() {
	studioSystem = nullptr;
	IsErroneous(Studio::System::create(&studioSystem));
	studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL);

	system = nullptr;
	studioSystem->getCoreSystem(&system);
	system->set3DSettings(1.0f, 1.0f, 50.0f);
}

AudioManager* AudioManager::GetInstance()
{
	if (AudioManager::inst == nullptr)
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
	FMOD_VECTOR* pos = VectorToFMODVector(position);
	FMOD_VECTOR* fr = VectorToFMODVector(forwardRot);
	FMOD_VECTOR* ur = VectorToFMODVector(upRot);
	system->set3DListenerAttributes(listenerID, pos, 0, fr, ur);
	delete pos;
	delete fr;
	delete ur;
}

/*Adds the sound into the sound map, if the name isn't already used.*/
void AudioManager::LoadSound(const std::string& soundName, bool threeDimensional, bool looping, bool stream) {

	//Create mode bitmask
	FMOD_MODE mode = FMOD_DEFAULT;
	mode = mode | (threeDimensional ? FMOD_3D : FMOD_2D);
	mode = mode | (looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	mode = mode | (stream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE);
	mode = (mode | FMOD_3D_LINEARROLLOFF);

	FMOD::Sound* newSound = nullptr;
	IsErroneous(system->createSound(soundName.c_str(), mode, nullptr, &newSound));
	
	if (newSound) { sounds[soundName] = newSound; };
}

int AudioManager::StartPlayingSound(const std::string& soundName, const Vector3& position, const float& volumePercent, const float& positionInSong) {
	int newChannel = nextChannelID;
	FMOD::Channel* channel = nullptr;
	Sound* sound = FindSound(soundName);
	if (sound != nullptr) {
		IsErroneous(system->playSound(sounds[soundName], nullptr, true, &channel));
		if (channel) {
			FMOD_MODE mode;
			sound->getMode(&mode);
			//if the 3d flag is set, update positional audio
			if (mode & FMOD_3D) {
				FMOD_VECTOR* posF = VectorToFMODVector(position);
				channel->set3DAttributes(posF, 0);
				delete posF;
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

/*AudioManager& AudioManager::SetChannelCallback(int channelID, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, std::function<void> callbackFunction) {
	FMOD::Channel* channel = FindChannel(channelID);
}*/

int AudioManager::FadeInSound(const std::string& soundName, float fadeInTime, const Vector3& position, const float& volumePercent, const float& positionInSong) {
	int newChannel = nextChannelID;
	unsigned long long dspclock;
	int softRate;
	FMOD::Channel* channel = nullptr;
	Sound* sound = FindSound(soundName);
	if (sound != nullptr) {
		IsErroneous(system->playSound(sounds[soundName], nullptr, true, &channel));
		if (channel) {
			FMOD_MODE mode;
			sound->getMode(&mode);
			//if the 3d flag is set, update positional audio
			if (mode & FMOD_3D) {
				FMOD_VECTOR* posF = VectorToFMODVector(position);
				channel->set3DAttributes(posF, 0);
				delete posF;
			}
			system->getSoftwareFormat(&softRate, 0, 0);
			channel->getDSPClock(0, &dspclock);
			channel->addFadePoint(dspclock, 0.0f);
			channel->addFadePoint(dspclock + (softRate * fadeInTime), volumePercent);
			channel->setPaused(false);
			channels.insert(std::pair<int, Channel*>(nextChannelID, channel));
		}
		nextChannelID++;
		return newChannel;
	}
	std::cout << "FMOD Sound hasn't been loaded - " << soundName << std::endl;
	return -1;
}

AudioManager& AudioManager::SetChannelVolume(int channelID, const float& volPercent) {
	Channel* channel = FindChannel(channelID);
	if (channel != nullptr) {
		IsErroneous(channel->setVolume(volPercent));
	}
	return *inst;
}

AudioManager& AudioManager::SetChannelPitch(int channelID, const float& pitch){
	Channel* channel = FindChannel(channelID);
	if (channel != nullptr) {
		IsErroneous(channel->setPitch(pitch));
	}
	return *inst;
}

AudioManager& AudioManager::SetChannel3DPos(int channelID, const Vector3& position) {
	Channel* channel = FindChannel(channelID);
	if (channel != nullptr) {
		FMOD_VECTOR* Fpos = VectorToFMODVector(position);
		IsErroneous(channel->set3DAttributes(Fpos, nullptr));
		delete Fpos;
	}
	return *inst;
}

void AudioManager::StopChannel(int channelID) {
	Channel* channel = FindChannel(channelID);
	if (channel != nullptr) {
		IsErroneous(channel->stop());
	}
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

FMOD_VECTOR* AudioManager::VectorToFMODVector(const Vector3& v) {
	return new FMOD_VECTOR{ v.x, v.y, v.z };
}

bool AudioManager::IsErroneous(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		std::cout << "FMOD Error No: " << result << std::endl;
		return true;
	}
	return true;
}
#endif // !_ORBIS
