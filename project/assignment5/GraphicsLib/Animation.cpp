#include "Animation.h"

Animation::Animation(bool doLoop)
{
    mCurrentSprite = 0;
    mTimePerFrame = convertFrametoMilliSec(BASE_FPS);
    mTimeUntilNextFrame = mTimePerFrame;
    mIsLooping = doLoop;
}

Animation::~Animation()
{
    mSpriteList.clear();
}

void Animation::addSprite(Sprite sprite)
{
    mSpriteList.push_back(sprite);
}

void Animation::update(double dt)
{
    // Guard: nothing to animate if empty
    if (mSpriteList.empty() || mIsFrozen) return;

    mTimeUntilNextFrame -= (float)dt;
    if (mTimeUntilNextFrame <= 0.0f)
    {
        // Advance frame
        if (mCurrentSprite < mSpriteList.size() - 1)
        {
            mCurrentSprite++;
        }
        else if (mIsLooping)
        {
            // Last frame reached on a looping animation wrap back
            reset();
        }
        // Non-looping animations stay on the last frame forever

        mTimeUntilNextFrame = mTimePerFrame;
    }
}
