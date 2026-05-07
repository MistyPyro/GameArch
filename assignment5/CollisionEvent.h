#pragma once
#include "GameEvent.h"
#include "PhysicsManager.h"
#include "Vector2D.h"
#include <string>

using namespace std;

class CollisionEvent : public GameEvent
{
public:
    CollisionEvent(PhysicsManager::Collision data)
        : GameEvent(COLLISION_EVENT)
        , mData(data)
    {}
    ~CollisionEvent() {};

    const string& getCollider1Tag() const { return mData.box1; };
    const string& getCollider2Tag() const { return mData.box2; };
    const Vector2D getResolution() const { return mData.resolution; };

private:
    PhysicsManager::Collision mData;
};