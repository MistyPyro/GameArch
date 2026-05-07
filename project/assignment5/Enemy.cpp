#include "Enemy.h"
#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include "CollisionEvent.h"
#include <cmath>

Enemy::Enemy(std::vector<Animation> walkAnim,
    std::vector<Animation> attackAnim,
    std::vector<Animation> dyingAnim,
    Vector2D startPos,
    float moveSpeed,
    int maxHp,
    int attackDamage,
    float attackRange,
    float attackCooldownMs,
    EnemyType type)
    : mWalkAnims(walkAnim)
    , mAttackAnims(attackAnim)
    , mDyingAnims(dyingAnim)
    , mPosition(startPos)
    , mMoveSpeed(moveSpeed)
    , mHp(maxHp)
    , mMaxHp(maxHp)
    , mAttackDamage(attackDamage)
    , mAttackRange(attackRange)
    , mAttackCooldownMs(attackCooldownMs)
    , mType(type)
    , mDrawScale(type == EnemyType::STRONG ? 1.4f : 1.0f)
{
}

Animation* Enemy::getCurrentAnim()
{
    switch (mState)
    {
    case EnemyState::ATTACKING:
        return mAttackAnims.empty() ? nullptr : &mAttackAnims[0];
    case EnemyState::DYING:
    case EnemyState::DEAD:
        return mDyingAnims.empty() ? nullptr : &mDyingAnims[0];
    default:
        return mWalkAnims.empty() ? nullptr : &mWalkAnims[0];
    }
}

void Enemy::setState(EnemyState newState)
{
    if (mState == newState) return;
    mState = newState;

    switch (mState)
    {
    case EnemyState::WALKING:
        if (!mWalkAnims.empty())   mWalkAnims[0].reset();
        break;
    case EnemyState::ATTACKING:
        if (!mAttackAnims.empty()) mAttackAnims[0].reset();
        break;
    case EnemyState::DYING:
        mDyingTimer = 0.0f;
        if (!mDyingAnims.empty())  mDyingAnims[0].reset();
        break;
    case EnemyState::DEAD:
        break;
    }
}

void Enemy::takeDamage(int amount)
{
    if (mState == EnemyState::DYING || mState == EnemyState::DEAD) return;
    mHp -= amount;
    if (mHp <= 0)
    {
        mHp = 0;
        setState(EnemyState::DYING);
    }
}

bool Enemy::didAttackLand()
{
    if (mAttackLanded)
    {
        mAttackLanded = false;
        return true;
    }
    return false;
}

void Enemy::update(double dt, Vector2D playerPos)
{
    if (mState == EnemyState::DEAD) return;

    Animation* pAnim = getCurrentAnim();
    if (pAnim) pAnim->update(dt);

    if (mState == EnemyState::DYING)
    {
        mDyingTimer += (float)dt;
        if (mDyingTimer >= DYING_DURATION_MS)
            setState(EnemyState::DEAD);
        return;
    }

    float dx = playerPos.getX() - mPosition.getX();
    float dy = playerPos.getY() - mPosition.getY();
    float dist = std::sqrt(dx * dx + dy * dy);

    if (mState == EnemyState::ATTACKING)
    {
        mAttackCooldownTimer -= (float)dt;
        if (dist > mAttackRange)
            setState(EnemyState::WALKING);
        else if (mAttackCooldownTimer <= 0.0f)
        {
            mAttackLanded = true;
            mAttackCooldownTimer = mAttackCooldownMs;
        }
        return;
    }

   if (dist <= mAttackRange)
    {
        mAttackCooldownTimer = 0.0f;
        setState(EnemyState::ATTACKING);
    }
    else if (dist > 0.0f)
    {
        float dtSec = (float)dt / 1000.0f;
        mPosition = Vector2D(
            mPosition.getX() + (dx / dist) * mMoveSpeed * dtSec,
            mPosition.getY() + (dy / dist) * mMoveSpeed * dtSec);
    }
}

void Enemy::handleEvent(const Event& theEvent)
{
    if (mState == EnemyState::DYING || mState == EnemyState::DEAD) return;

    switch (theEvent.getType())
    {
    case COLLISION_EVENT:
    {
        const CollisionEvent& col = static_cast<const CollisionEvent&>(theEvent);
        std::string myKey = "enemy" + std::to_string((uintptr_t)this);

        // Check this collision involves this specific enemy
        bool involvesMe = (col.getCollider1Tag() == myKey ||
            col.getCollider2Tag() == myKey);
        if (!involvesMe) return;

        // Check if the other collider is the punch hitbox
        bool punchHit = (col.getCollider1Tag() == "playerPunch" ||
            col.getCollider2Tag() == "playerPunch");

        if (punchHit)
        {
            takeDamage(PUNCH_DAMAGE);
            std::cout << "Enemy hit by punch!\n";
        }
        break;
    }
    default:
        break;
    }
}

void Enemy::draw(Vector2D cameraOffset)
{
    if (mState == EnemyState::DEAD) return;

    Animation* pAnim = getCurrentAnim();
    if (!pAnim) return;

    // Apply camera offset
    Vector2D screenPos(mPosition.getX() - cameraOffset.getX(),
        mPosition.getY() - cameraOffset.getY());

    // Strong enemies draw at 1.4x scale so they look visually distinct
    GraphicsSystem::getBackBuffer()->draw(screenPos, pAnim->getCurrentSprite(), mDrawScale);
}

