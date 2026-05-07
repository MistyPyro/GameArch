#include "Unit.h"
#include "Animation.h"
#include "GraphicsBuffer.h"
#include "GraphicsSystem.h"
#include "Sprite.h"

// dt is in milliseconds throughout the game loop.
// Movement uses seconds, so we convert: dtSec = dt / 1000.0
static constexpr double MS_TO_SEC = 1.0 / 1000.0;

Unit::Unit(std::vector<Animation> pAnimationVector,
    unsigned int width, unsigned int height, float moveSpeed)
    : mpAnimations(pAnimationVector)
    , mSceneWidth(width)
    , mSceneHeight(height)
    , mMoveSpeed(moveSpeed)
{
    mpCurrentAnimation = &mpAnimations[0];
    mLocation = Vector2D(0, 0);
    mIsFrozen = false;
    mCurrentAnimationIndex = 0;
}

Unit::~Unit() {}

void Unit::update(double dt, Vector2D /*dir*/)
{
   /* mpCurrentAnimation->update(dt);

    if (!mIsFrozen)
    {
        // Convert dt from milliseconds to seconds for world-space movement
        double dtSec = dt * MS_TO_SEC;
        setPosition(mLocation + (mDir.getNormalizedVector() * mMoveSpeed * (float)dtSec));
    }*/

    mpCurrentAnimation->update(dt);
    if (!mIsFrozen)
    {
        setPosition(mLocation + (mDir.getNormalizedVector() * mMoveSpeed * (float)dt));
    }
}

void Unit::toggleFrozen()
{
    getCurrentAnimation()->toggleFrozen();
}

void Unit::switchAnimations()
{
    if (mpAnimations.empty()) return;

    if (mpCurrentAnimation == &mpAnimations.back())
    {
        mpCurrentAnimation = &mpAnimations.front();
        mCurrentAnimationIndex = 0;
    }
    else
    {
        mpCurrentAnimation = &mpAnimations[++mCurrentAnimationIndex];
    }
}

void Unit::draw(Vector2D cameraOffset)
{
    Vector2D screenPos(mLocation.getX() - cameraOffset.getX(),
        mLocation.getY() - cameraOffset.getY());
    GraphicsSystem::getBackBuffer()->draw(screenPos,
        mpCurrentAnimation->getCurrentSprite());
}







