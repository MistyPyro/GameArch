#pragma once
#include "InputEvent.h"
#include "Vector2D.h"

class MouseEvent : public InputEvent
{
public:
	MouseEvent(const Vector2D& loc, int keyCode, bool pressed);
	~MouseEvent();

	Vector2D getLocation() const { return mLoc; };
	inline bool isPressed() const { return mPressed; };
	inline bool isReleased() const { return !mPressed; };

private:
	Vector2D mLoc;
	bool mPressed;
};