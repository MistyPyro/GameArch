#include "Boss.h"
#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include "EventSystem.h"
#include "BossEvent.h"
#include "ScoreEvent.h"
#include <cmath>
#include <cstdlib>
#include <iostream>


static constexpr float HITBOX_ACTIVE_DURATION_MS = 180.0f;

static constexpr float ENGAGE_DISTANCE = 180.0f;

static constexpr float BOSS_MOVE_SPEED_DEFAULT = 120.0f;

static constexpr float SHARK_CHARGE_SPEED = 400.0f;
static constexpr float CHARGE_FORWARD_SPEED = 250.0f;

static constexpr float HURT_FLASH_MS = 200.0f;

static constexpr float PHASE2_RANGE_THRESHOLD = 250.0f;
static constexpr float PHASE1_RANGE_THRESHOLD = 200.0f;


Boss::Boss(std::vector<Animation> idleAnim,
    std::vector<Animation> walkAnim,
    std::vector<Animation> attackAnims,
    std::vector<Animation> hurtAnim,
    std::vector<Animation> dyingAnim,
    Vector2D startPos, int maxHp)
    : mIdleAnims(idleAnim)
    , mWalkAnims(walkAnim)
    , mAttackAnims(attackAnims)
    , mHurtAnims(hurtAnim)
    , mDyingAnims(dyingAnim)
    , mPosition(startPos)
    , mHp(maxHp)
    , mMaxHp(maxHp)
    , mMoveSpeed(BOSS_MOVE_SPEED_DEFAULT)
{
}

Animation* Boss::getCurrentAnim()
{
    switch (mState)
    {
    case BossState::WALKING:
        return mWalkAnims.empty() ? nullptr : &mWalkAnims[0];

    case BossState::ATTACKING:
    {
        int idx = (int)mCurrentAttack - 1;
        if (idx >= 0 && idx < (int)mAttackAnims.size())
            return &mAttackAnims[idx];
        return mIdleAnims.empty() ? nullptr : &mIdleAnims[0];
    }

    case BossState::HURT:
        return mHurtAnims.empty() ? nullptr : &mHurtAnims[0];

    case BossState::DYING:
    case BossState::DEAD:
        return mDyingAnims.empty() ? nullptr : &mDyingAnims[0];

    default: // IDLE
        return mIdleAnims.empty() ? nullptr : &mIdleAnims[0];
    }
}

float Boss::attackDuration(BossAttack a) const
{
    switch (a)
    {
    case BossAttack::SIMPLE_JAB: return 400.0f;
    case BossAttack::BIG_SWING: return 900.0f;
    case BossAttack::TONGUE_ATTACK: return 700.0f;
    case BossAttack::LARGE_FORWARD: return 600.0f;
    case BossAttack::SHARK_CHARGE: return 500.0f;
    case BossAttack::BIG_PUNCH: return 800.0f;
    case BossAttack::DOWNWARD_ATTACK: return 700.0f;
    case BossAttack::DOWN_SPIKE: return 600.0f;
    case BossAttack::CHARGE_FORWARD: return 800.0f;
    case BossAttack::TENTACLE_ATTACK: return 1200.0f;
    default: return  500.0f;
    }
}

float Boss::hitboxStart(BossAttack a) const
{
    switch (a)
    {
    case BossAttack::SIMPLE_JAB: return 150.0f;
    case BossAttack::BIG_SWING: return 300.0f;
    case BossAttack::TONGUE_ATTACK: return 200.0f;
    case BossAttack::LARGE_FORWARD: return 200.0f;
    case BossAttack::SHARK_CHARGE: return  50.0f;
    case BossAttack::BIG_PUNCH: return 350.0f;
    case BossAttack::DOWNWARD_ATTACK: return 300.0f;
    case BossAttack::DOWN_SPIKE: return 200.0f;
    case BossAttack::CHARGE_FORWARD: return 100.0f;
    case BossAttack::TENTACLE_ATTACK: return 200.0f;
    default: return 200.0f;
    }
}

float Boss::hitboxEnd(BossAttack a) const
{
    // Hitbox closes HITBOX_ACTIVE_DURATION_MS after it opens
    return hitboxStart(a) + HITBOX_ACTIVE_DURATION_MS;
}

float Boss::cooldownAfter(BossAttack a) const
{
    // Higher phase = shorter cooldown (faster boss)
    float mult = (mPhase == BossPhase::ONE) ? 1.0f : (mPhase == BossPhase::TWO) ? 0.75f : 0.55f;
    switch (a)
    {
    case BossAttack::SIMPLE_JAB: return  800.0f * mult;
    case BossAttack::BIG_SWING: return 1500.0f * mult;
    case BossAttack::SHARK_CHARGE: return  600.0f * mult;
    case BossAttack::TENTACLE_ATTACK: return 2000.0f * mult;
    default: return 1200.0f * mult;
    }
}

float Boss::getAttackRange() const
{
    switch (mCurrentAttack)
    {
    case BossAttack::TONGUE_ATTACK: return 320.0f;
    case BossAttack::SHARK_CHARGE: return 260.0f;
    case BossAttack::CHARGE_FORWARD: return 240.0f;
    case BossAttack::TENTACLE_ATTACK: return 200.0f;
    case BossAttack::BIG_SWING: return 160.0f;
    case BossAttack::DOWNWARD_ATTACK: return 120.0f;
    default: return 110.0f;
    }
}

int Boss::getAttackDamage() const
{
    switch (mCurrentAttack)
    {
    case BossAttack::SIMPLE_JAB: return 1;
    case BossAttack::BIG_SWING: return 3;
    case BossAttack::TONGUE_ATTACK: return 2;
    case BossAttack::LARGE_FORWARD: return 2;
    case BossAttack::SHARK_CHARGE: return 3;
    case BossAttack::BIG_PUNCH: return 3;
    case BossAttack::DOWNWARD_ATTACK: return 2;
    case BossAttack::DOWN_SPIKE: return 2;
    case BossAttack::CHARGE_FORWARD: return 2;
    case BossAttack::TENTACLE_ATTACK: return 4;
    default: return 1;
    }
}


void Boss::setState(BossState newState)
{
    if (mState == newState) return;
    mState = newState;
    Animation* pAnim = getCurrentAnim();
    if (pAnim) pAnim->reset();
}

void Boss::setAttack(BossAttack attack)
{
    mCurrentAttack = attack;
    mAttackTimer = 0.0f;
    mAttackDuration = attackDuration(attack);
    mHitboxActive = false;
    mHitDealtThisSwing = false;

    int idx = (int)attack - 1;
    if (idx >= 0 && idx < (int)mAttackAnims.size())
        mAttackAnims[idx].reset();

    setState(BossState::ATTACKING);
    EventSystem::getInstance()->fireEvent(BossAttackEvent((int)attack));
}

void Boss::chooseNextAttack(Vector2D playerPos)
{
    float dist = std::sqrt(
        std::pow(playerPos.getX() - mPosition.getX(), 2) +
        std::pow(playerPos.getY() - mPosition.getY(), 2));

    if (mPhase == BossPhase::THREE)
    {
        // Phase 3: fully random from all 10 attacks
        setAttack((BossAttack)(rand() % 10 + 1));
        return;
    }

    if (mPhase == BossPhase::TWO)
    {
        if (dist > PHASE2_RANGE_THRESHOLD)
            setAttack((rand() % 2 == 0) ? BossAttack::SHARK_CHARGE
                : BossAttack::CHARGE_FORWARD);
        else
        {
            static const BossAttack phase2Attacks[] = {
                BossAttack::BIG_SWING, BossAttack::TONGUE_ATTACK,
                BossAttack::BIG_PUNCH, BossAttack::TENTACLE_ATTACK,
                BossAttack::DOWNWARD_ATTACK
            };
            setAttack(phase2Attacks[rand() % 5]);
        }
        return;
    }

    // Phase 1
    if (dist > PHASE1_RANGE_THRESHOLD)
    {
        setAttack(BossAttack::SHARK_CHARGE);
    }
    else
    {
        static const BossAttack phase1Attacks[] = {
            BossAttack::SIMPLE_JAB, BossAttack::BIG_SWING,
            BossAttack::LARGE_FORWARD, BossAttack::DOWN_SPIKE
        };
        setAttack(phase1Attacks[rand() % 4]);
    }
}

void Boss::updatePhase()
{
    float pct = (float)mHp / (float)mMaxHp;
    BossPhase newPhase = (pct <= 0.33f) ? BossPhase::THREE
        : (pct <= 0.66f) ? BossPhase::TWO
        : BossPhase::ONE;

    if (newPhase != mPhase)
    {
        mPhase = newPhase;
        EventSystem::getInstance()->fireEvent(BossPhaseEvent((int)mPhase + 1));
        std::cout << "Boss phase " << ((int)mPhase + 1) << "!\n";
    }
}

void Boss::takeDamage(int amount)
{
    if (mState == BossState::DYING || mState == BossState::DEAD) return;

    mHp -= amount;
    if (mHp <= 0)
    {
        mHp = 0;
        setState(BossState::DYING);
        mDyingTimer = 0.0f;
        mDyingTimer = 0.0f;
        EventSystem::getInstance()->fireEvent(BossDeadEvent());
        EventSystem::getInstance()->fireEvent(ScoreEvent(BOSS_KILL_SCORE));
        std::cout << "BOSS DEFEATED!\n";
        return;
    }

    updatePhase();
    if (mState != BossState::ATTACKING)
    {
        setState(BossState::HURT);
        mHurtTimer = HURT_FLASH_MS;
    }
}


void Boss::update(double dt, Vector2D playerPos)
{
    if (mState == BossState::DEAD) return;

    Animation* pAnim = getCurrentAnim();
    if (pAnim) pAnim->update(dt);

    if (mState == BossState::DYING)
    {
        mDyingTimer += (float)dt;
        if (mDyingTimer >= DYING_DURATION)
            setState(BossState::DEAD);
        return;
    }

    if (mState == BossState::HURT)
    {
        mHurtTimer -= (float)dt;
        if (mHurtTimer <= 0.0f)
        {
            mHurtTimer = 0.0f;
            setState(BossState::IDLE);
        }
        return;
    }

    if (mState == BossState::ATTACKING)
    {
        mAttackTimer += (float)dt;

        float hs = hitboxStart(mCurrentAttack);
        float he = hitboxEnd(mCurrentAttack);
        mHitboxActive = (mAttackTimer >= hs && mAttackTimer < he
            && !mHitDealtThisSwing);

        // Charge-type attacks move the boss towards the player while active
        if (mHitboxActive)
        {
            float dx = playerPos.getX() - mPosition.getX();
            float dy = playerPos.getY() - mPosition.getY();
            float dist = std::sqrt(dx * dx + dy * dy);

            if ((mCurrentAttack == BossAttack::SHARK_CHARGE ||
                mCurrentAttack == BossAttack::CHARGE_FORWARD) && dist > 1.0f)
            {
                float speed = (mCurrentAttack == BossAttack::SHARK_CHARGE)
                    ? SHARK_CHARGE_SPEED : CHARGE_FORWARD_SPEED;
                float dtSec = (float)dt / 1000.0f;
                mPosition = Vector2D(
                    mPosition.getX() + (dx / dist) * speed * dtSec,
                    mPosition.getY() + (dy / dist) * speed * dtSec);
            }
        }

        // Attack finished — enter cooldown
        if (mAttackTimer >= mAttackDuration)
        {
            mHitboxActive = false;
            BossAttack finishedAttack = mCurrentAttack;
            mCurrentAttack = BossAttack::NONE;
            mCooldownTimer = cooldownAfter(finishedAttack);
            setState(BossState::IDLE);
        }
        return;
    }

    if (mCooldownTimer > 0.0f)
    {
        mCooldownTimer -= (float)dt;
        setState(BossState::WALKING); // walk idly during cooldown
        return;
    }

    float dx = playerPos.getX() - mPosition.getX();
    float dy = playerPos.getY() - mPosition.getY();
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist > ENGAGE_DISTANCE)
    {
        // Move towards player
        setState(BossState::WALKING);
        float dtSec = (float)dt / 1000.0f;
        mPosition = Vector2D(
            mPosition.getX() + (dx / dist) * mMoveSpeed * dtSec,
            mPosition.getY() + (dy / dist) * mMoveSpeed * dtSec);
    }
    else
    {
        // Close enough — pick and start an attack
        chooseNextAttack(playerPos);
    }
}

void Boss::draw(Vector2D cameraOffset)
{
    if (mState == BossState::DEAD) return;

    Animation* pAnim = getCurrentAnim();
    if (!pAnim) return;

    Vector2D screenPos(mPosition.getX() - cameraOffset.getX(),
        mPosition.getY() - cameraOffset.getY());
    GraphicsSystem::getBackBuffer()->draw(screenPos, pAnim->getCurrentSprite());
}






