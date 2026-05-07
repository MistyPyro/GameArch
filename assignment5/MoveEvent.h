#pragma once
#include <Vector2D.h>
#include "GameEvent.h"

class MoveEvent : public GameEvent
{
public:
	MoveEvent(const Vector2D& loc, const bool isEnd);
	~MoveEvent();

	Vector2D getLocation() const { return mLoc; };
	inline bool isEnd() const { return mIsEnd; };

private:
	Vector2D mLoc;
	bool mIsEnd;
};