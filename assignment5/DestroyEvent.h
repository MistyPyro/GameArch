#pragma once
#include "GameEvent.h"
#include "Vector2D.h"

class DestroyEvent : public GameEvent
{
public:
	DestroyEvent(const Vector2D loc);
	~DestroyEvent();

	Vector2D getLocation() const { return mLoc; };

private:
	Vector2D mLoc;
};