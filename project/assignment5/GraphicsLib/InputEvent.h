#pragma once
#include "Event.h"

enum InputEventType
{
	KEY_DOWN = NUM_BASE_EVENT_TYPES,
	KEY_UP,
	MOUSE_ACTION_EVENT,
	NUM_INPUT_EVENT_TYPES
};

class InputEvent : public Event
{
public:
	InputEvent(InputEventType type, int keyCode);
	virtual ~InputEvent();

	inline int getKeyCode() const { return mKeyCode; };

private:
	int mKeyCode;
};