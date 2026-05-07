#pragma once
#include "GameEvent.h"
#include "Vector2D.h"

class WebFireEvent :public GameEvent
{
public:
	WebFireEvent(const Vector2D& origin, const Vector2D& target) : GameEvent(WEB_FIRE_EVENT), mOrignin(origin), mTarget(target) {}
	~WebFireEvent() {}
	Vector2D getOrigin() const { return mOrignin; }
	Vector2D getTarget() const { return mTarget; }

private:
	Vector2D mOrignin;
	Vector2D mTarget;
};

class WebHitEvent :public GameEvent
{
public:
	WebHitEvent() : GameEvent(WEB_HIT_EVENT) {}
	~WebHitEvent() {}
};

class WebMeterEmptyEvent :public GameEvent
{
public:
	WebMeterEmptyEvent() : GameEvent(WEB_METER_EMPTY_EVENT) {}
	~WebMeterEmptyEvent() {}
};

class WebMeterRefilledEvent :public GameEvent
{
public:
	WebMeterRefilledEvent() : GameEvent(WEB_METER_REFILLED_EVENT) {}
	~WebMeterRefilledEvent() {}
};

class WebBlastEvent : public GameEvent
{
public:
	WebBlastEvent(const Vector2D& origin, const Vector2D& target) : GameEvent(WEB_BLAST_EVENT), mOrigin(origin), mTarget(target) {
	}
	~WebBlastEvent() {}
	Vector2D getOrigin() const { return mOrigin; }
	Vector2D getTarget() const { return mTarget; }
private:
	Vector2D mOrigin;
	Vector2D mTarget;
};

class BlockEvent : public GameEvent
{
public:
	BlockEvent(bool isBlocking) : GameEvent(BLOCK_EVENT), mBlocking(isBlocking) 
	{
	}
	~BlockEvent() {}
	bool isBlocking() const { return mBlocking; }

private:
	bool mBlocking;
};

