#include "AudioManager.h"
#include "../GameEvent.h"
#include "EventSystem.h"
#include "../ConsumingEvent.h"
#include <iostream>

// https://wiki.libsdl.org/SDL3/FrontPage
// https://wiki.libsdl.org/wiki/search
// https://wiki.libsdl.org/SDL3/QuickReference
//https://wiki.libsdl.org/SDL3/CategoryAudio
//https://examples.libsdl.org/SDL3/audio/03-load-wav/
//https://stackoverflow.com/questions/50240497/sdl-how-to-play-audio-asynchronously-in-c-without-stopping-code-execution
//https://examples.libsdl.org/SDL3/audio/04-multiple-streams/
//https://wiki.libsdl.org/SDL3/Tutorials


using namespace std;

AudioManager::AudioManager()
{
}

bool AudioManager::init()
{
	if (!SDL_Init(SDL_INIT_AUDIO))
	{
		cout << "AudioManagerSDL_Init(AUDIO) error: " << "\n";
		return false;
	}

	SDL_AudioSpec desired;
	desired.freq = 44100;
	desired.format = SDL_AUDIO_S16LE;
	desired.channels = 2;

	mDeviceID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired);
	if (mDeviceID == 0)
	{
		cout << "AudioManager: SDL_OpenAudioDevice error: " << "\n";
		return false;
	}


	EventSystem::getInstance()->addListener((EventType)MENU_SOUND_EVENT, this);
	EventSystem::getInstance()->addListener((EventType)MENU_MUSIC_EVENT, this);
	EventSystem::getInstance()->addListener((EventType)CONSUMING_EVENT, this);

	cout << "AudioManager initialized.\n";
	return true;
}

void AudioManager::cleanup()
{
	EventSystem::getInstance()->removeListenerFromAllEvents(this);
	for (auto& pair : mMap)
	{
		SDL_free(pair.second->buffer);
		delete pair.second;
	}
	mMap.clear();
	mPending.clear();

	if (mDeviceID != 0)
	{
		SDL_CloseAudioDevice(mDeviceID);
		mDeviceID = 0;
	}

	for (SDL_AudioStream* s : mStreams)
	{
		if (s)
		{
			SDL_ClearAudioStream(s);
			SDL_UnbindAudioStream(s);
			SDL_DestroyAudioStream(s);
		}
		
	}
	mStreams.clear();
}

bool AudioManager::loadSound(const SoundKey& key, const std::string& filename)
{
	if (mMap.find(key) != mMap.end())
	{
		return true;
	}

	SoundData* pData = new SoundData();

	if (!SDL_LoadWAV(filename.c_str(), &pData->spec, &pData->buffer, &pData->length))
	{
		cout << "AudioManager: SDL_LoadWAV error (" << filename << "): " << "\n";
		delete pData;
		return false;
	}

	mMap[key] = pData;
	cout << "AudioManager: loaded \"" << key << "\" from " << filename << "\n";
	return true;
}

void AudioManager::playSound(const SoundKey& key)
{


	auto iter = mMap.find(key);
	if (iter == mMap.end())
	{
		cout << "AudioManager: sound \"" << key << "\" not found.\n";
		return;
	}

	SoundData* pData = iter->second;
	SDL_AudioStream* pStream = SDL_CreateAudioStream(&pData->spec, &pData->spec);
	if (!pStream)
	{
		cout << "AudioManager: SDL_CreateAudioStream error: " << "\n";
		return;
	}

	SDL_PutAudioStreamData(pStream, pData->buffer, (int)pData->length);
	SDL_FlushAudioStream(pStream);
	SDL_BindAudioStream(mDeviceID, pStream);
	mStreams.push_back(pStream);
}

void AudioManager::update(double dtMs)
{
	for (int i = (int)mStreams.size() - 1; i >= 0; i--)
	{
		if (SDL_GetAudioStreamAvailable(mStreams[i]) == 0)
		{
			SDL_UnbindAudioStream(mStreams[i]);
			SDL_DestroyAudioStream(mStreams[i]);
			mStreams.erase(mStreams.begin() + i);
		}
	}

	for (int i = (int)mPending.size() - 1; i >= 0; i--)
	{
		mPending[i].remainingMs -= (float)dtMs;
		if (mPending[i].remainingMs <= 0.0f)
		{
			playSound(mPending[i].key);
			mPending.erase(mPending.begin() + i);
		}
	}
}

void AudioManager::handleEvent(const Event& theEvent)
{
	switch (theEvent.getType())
	{
	case CONSUMING_EVENT:
	{
		const ConsumingEvent& consumingEvent =
			static_cast<const ConsumingEvent&>(theEvent);
		float delay = consumingEvent.getDelay();
		std::string key = consumingEvent.getSoundKey();

		if (delay <= 0.0f)
			playSound(key);
		else
			mPending.push_back({ key, delay });
		break;
	}
	default:
		break;
	}
}