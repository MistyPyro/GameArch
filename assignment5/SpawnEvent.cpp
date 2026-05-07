#include "SpawnEvent.h"

SpawnEvent::SpawnEvent(const Vector2D loc)
	:GameEvent(SPAWN_EVENT)
	,mLoc(loc)
{
}

SpawnEvent::~SpawnEvent()
{
}