#include "InputEvent.h"

InputEvent::InputEvent(InputEventType type, int keyCode)
	:Event((EventType)type),
	mKeyCode(keyCode)
{
}

InputEvent::~InputEvent()
{
}