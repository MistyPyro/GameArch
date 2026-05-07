#pragma once
#include "GameEvent.h"

class ScoreEvent : public GameEvent
{
public:
	ScoreEvent(int amount) : GameEvent(SCORE_EVENT), mAmount(amount)
	{
	}
	~ScoreEvent() {}

	int getAmount() const { return mAmount; }

private:
	int mAmount;
};
