#pragma once
#include <Trackable.h>
#include <vector>
#include "Sprite.h"

class Animation : public Trackable
{
public:
	Animation(bool doLoop);
	~Animation();
	void addSprite(Sprite sprite);
	float convertFrametoMilliSec(float fps) { return (float)(1000.0 / fps); };
	void toggleFrozen() { mIsFrozen = !mIsFrozen; };
	void update(double dt);
	Sprite getCurrentSprite() { return mSpriteList[mCurrentSprite]; };
	void setFrameRate(float frame) { mTimePerFrame = convertFrametoMilliSec(frame); }
	void reset() { mCurrentSprite = 0; }

    Animation(const Animation& other) : mSpriteList(other.mSpriteList), mIsLooping(other.mIsLooping), mIsFrozen(other.mIsFrozen), mTimePerFrame(other.mTimePerFrame), mTimeUntilNextFrame(other.mTimeUntilNextFrame), mCurrentSprite(other.mCurrentSprite)
    {
    }

    Animation& operator=(const Animation& other)
    {
        if (this != &other)
        {
            mSpriteList = other.mSpriteList;
            mIsLooping = other.mIsLooping;
            mIsFrozen = other.mIsFrozen;
            mTimePerFrame = other.mTimePerFrame;
            mTimeUntilNextFrame = other.mTimeUntilNextFrame;
            mCurrentSprite = other.mCurrentSprite;
        }
        return *this;
    }
private:
	std::vector<Sprite> mSpriteList;
	bool mIsLooping, mIsFrozen = false;
	float mTimePerFrame, mTimeUntilNextFrame;
	unsigned int mCurrentSprite;
	const float BASE_FPS = 15;
};