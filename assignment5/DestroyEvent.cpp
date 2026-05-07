#include "DestroyEvent.h"

DestroyEvent::DestroyEvent(const Vector2D loc)
	:GameEvent(DESTROY_EVENT)
	,mLoc(loc)
{
}

DestroyEvent::~DestroyEvent()
{
}