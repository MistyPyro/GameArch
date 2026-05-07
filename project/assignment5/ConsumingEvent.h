#pragma once
#include "GameEvent.h"
#include <string>

class ConsumingEvent : public GameEvent
{
public:
    ConsumingEvent(float delayMs = 0.0f, const std::string& soundKey = "consuming")
        : GameEvent(CONSUMING_EVENT)
        , mDelayMs(delayMs)
        , mSoundKey(soundKey)
    {

    }
    ~ConsumingEvent() {}

    float getDelay() const { return mDelayMs; }
    const std::string& getSoundKey() const { return mSoundKey; }

private:
    float mDelayMs;
    std::string mSoundKey;
};