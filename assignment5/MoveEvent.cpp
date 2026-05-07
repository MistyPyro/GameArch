#include "MoveEvent.h"

MoveEvent::MoveEvent(const Vector2D& loc, const bool isEnd)
	:GameEvent(MOVE_EVENT)
	, mLoc(loc)
	, mIsEnd(isEnd)
{
}

MoveEvent::~MoveEvent()
{
}