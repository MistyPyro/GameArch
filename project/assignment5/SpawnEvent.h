#pragma once
#include "GameEvent.h"
#include "Vector2D.h"

class SpawnEvent : public GameEvent
{
public:
	SpawnEvent(const Vector2D loc);
	~SpawnEvent();

	Vector2D getLocation() const { return mLoc; };

private:
	Vector2D mLoc;
};