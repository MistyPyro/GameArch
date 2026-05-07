#pragma once
#include "EventListener.h"
#include "unordered_map"
#include "vector"
#include "Sprite.h"

class Animation;

typedef std::string AnimationKey;

class AnimationManager : public EventListener
{
public:
	AnimationManager() {};
	~AnimationManager() { cleanup(); };

	void init() {};
	void cleanup();

	Animation* createAndAddAnimation(bool isLooping, std::vector<Sprite> spriteList, const AnimationKey& key);
	Animation* getAnimation(const AnimationKey& key);
	void toggleAnimationFreeze(const AnimationKey& key);

	void handleEvent(const Event& theEvent);
private:
	std::unordered_map<AnimationKey, Animation*> mMap;
};