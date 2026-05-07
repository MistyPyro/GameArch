#include "MouseEvent.h"

MouseEvent::MouseEvent(const Vector2D& loc, int keyCode, bool pressed)
	:InputEvent(MOUSE_ACTION_EVENT, keyCode)
	,mLoc(loc)
	,mPressed(pressed)
{
}

MouseEvent::~MouseEvent()
{
}