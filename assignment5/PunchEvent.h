#pragma once
#include "GameEvent.h"


class PunchEvent : public GameEvent
{
public:
	PunchEvent(bool isPunching) : GameEvent(PUNCH_EVENT), mPunching(isPunching)
	{
	}
	~PunchEvent() {}
	bool isPunching() const { return mPunching; }

private:
	bool mPunching;
};
