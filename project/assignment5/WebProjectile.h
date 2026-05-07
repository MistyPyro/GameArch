#pragma once
#include <Trackable.h>
#include "Vector2D.h"
#include "Object.h"

class WebProjectile : public Trackable, public Object
{
    friend class WebManager;
    friend class WebProjectilePool;
public:
    bool isActive() const { return mActive; }
    bool hasHit() const { return mHit; }
    Vector2D getPosition() const { return mPosition; }
    Vector2D getDirection() const { return mDirection; }
    float getSpeed() const { return mSpeed; }

private:
    WebProjectile()
        : mPosition(0, 0)
        , mDirection(1, 0)
        , mSpeed(600.0f)
        , mActive(false)
        , mHit(false)
        , mLifetimeMs(0.0f)
    {
    }
    ~WebProjectile() {}

    // Called by pool to fire this projectile
    void reset(Vector2D origin, Vector2D direction, float speed)
    {
        mPosition = origin;
        mDirection = direction;
        mSpeed = speed;
        mActive = true;
        mHit = false;
        mLifetimeMs = 0.0f;
    }

    void update(double dt);
    void draw(Vector2D cameraOffset = Vector2D(0, 0));
    Vector2D mPosition;
    Vector2D mDirection;
    float mSpeed;
    bool mActive;
    bool mHit;
    float mLifetimeMs;
    const float MAX_LIFETIME_MS = 1200.0f;
};

class WebProjectilePool
{
public:
    static const int POOL_SIZE = 32;  

    WebProjectilePool()
    {
        // Pre-allocate all slots
        for (int i = 0; i < POOL_SIZE; i++)
            mPool[i] = new WebProjectile();
    }
    ~WebProjectilePool()
    {
        for (int i = 0; i < POOL_SIZE; i++)
        {
            delete mPool[i];
            mPool[i] = nullptr;
        }
    }

    WebProjectile* acquire(Vector2D origin, Vector2D direction, float speed)
    {
        for (int i = 0; i < POOL_SIZE; i++)
        {
            if (!mPool[i]->mActive)
            {
                mPool[i]->reset(origin, direction, speed);
                return mPool[i];
            }
        }
        return nullptr;  // pool full caller can safely ignore
    }

    // Returns all projectiles so WebManager can iterate them
    WebProjectile* const* getAll() const { return mPool; }
    int size() const { return POOL_SIZE; }

private:
    WebProjectile* mPool[POOL_SIZE];
};
