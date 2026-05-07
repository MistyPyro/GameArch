#pragma once
#include "EventListener.h"
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
#include <vector>

typedef std::string SoundKey;

class AudioManager : public EventListener
{
public:
	AudioManager();
	~AudioManager() { cleanup(); };
	bool init();
	void cleanup();
	bool loadSound(const SoundKey& key, const std::string& filename);
	void playSound(const SoundKey& key);
	void handleEvent(const Event& theEvent);

	void update(double dtMs);

protected:
	SDL_AudioDeviceID mDeviceID = 0;

private:
	struct SoundData
	{
		SDL_AudioSpec spec = {};
		Uint8* buffer = nullptr;
		Uint32 length = 0;
	};

	struct PendingSound
	{
		SoundKey key;
		float remainingMs;
	};

	std::unordered_map<SoundKey, SoundData*> mMap;
	std::vector<PendingSound> mPending;
	std::vector<SDL_AudioStream*> mStreams;
};