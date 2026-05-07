#include "PlayerManager.h"
#include "Unit.h"
#include "MoveEvent.h"
#include "WebEvent.h"
#include "PunchEvent.h"
#include "GameEvent.h"
#include "EventSystem.h"
#include "GraphicsSystem.h"
#include "WebManager.h"
#include "GraphicsBuffer.h"
#include "CollisionEvent.h"
#include "MenuEvent.h"
#include "GameAudioManager.h"
#include <iostream>
#include "FileManager.h"
#include "Collider.h"

PlayerManager* PlayerManager::mpsInstance = nullptr;

PlayerManager* PlayerManager::createInstance()
{
    if (mpsInstance == nullptr)
        mpsInstance = new PlayerManager;
    return mpsInstance;
}

void PlayerManager::destroyInstance()
{
    delete mpsInstance;
    mpsInstance = nullptr;
}

PlayerManager::PlayerManager()
{
    EventSystem::getInstance()->addListener((EventType)MOVE_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)WEB_FIRE_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)WEB_BLAST_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)BLOCK_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)PUNCH_EVENT, this); 
    EventSystem::getInstance()->addListener((EventType)COLLISION_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_DIFFICULTY_EVENT, this);
}

void PlayerManager::init()
{
    FileManager::getInstance()->readAndImportFile(DATA_FILE);
}

void PlayerManager::cleanup()
{
    EventSystem::getInstance()->removeListenerFromAllEvents(this);
    if (mpPlayerUnit)
    {
        delete mpPlayerUnit;
        mpPlayerUnit = nullptr;
    }
    mpWebManager = nullptr;
    mpAudioManager = nullptr;
}

void PlayerManager::setAnimations(
    std::vector<Animation> idle,
    std::vector<Animation> walk,
    std::vector<Animation> shootWeb,
    std::vector<Animation> webBlast,
    std::vector<Animation> block,
    std::vector<Animation> punching,
    std::vector<Animation> dying)
{
    mIdleAnims = idle;
    mWalkAnims = walk;
    mShootAnims = shootWeb;
    mBlastAnims = webBlast;
    mBlockAnims = block;
    mPunchingAnims = punching;
    mDyingAnims = dying;
    mAnimsLoaded = true;
}

void PlayerManager::createPlayer(std::vector<Animation> fallbackAnims, Vector2D pos, float moveSpeed)
{
    mpPlayerUnit = new Unit(fallbackAnims, 0, 0, moveSpeed);
    mpPlayerUnit->setPosition(pos);
    PhysicsManager::getInstance()->createCollider("player", PLAYER_COLLIDER_SIZE, PLAYER_COLLIDER_SIZE, this->getPosition(), false, this);
}

Animation* PlayerManager::getCurrentAnim()
{
    if (!mAnimsLoaded) return nullptr;
    switch (mState)
    {
    case PlayerState::WALKING: return mWalkAnims.empty() ? nullptr : &mWalkAnims[0];
    case PlayerState::SHOOTING_WEB: return mShootAnims.empty() ? nullptr : &mShootAnims[0];
    case PlayerState::WEB_BLAST: return mBlastAnims.empty() ? nullptr : &mBlastAnims[0];
    case PlayerState::BLOCKING: return mBlockAnims.empty() ? nullptr : &mBlockAnims[0];
    case PlayerState::PUNCHING: return mPunchingAnims.empty() ? nullptr : &mPunchingAnims[0];
    case PlayerState::DYING:
    case PlayerState::DEAD: return mDyingAnims.empty() ? nullptr : &mDyingAnims[0];
    default: return mIdleAnims.empty() ? nullptr : &mIdleAnims[0];
    }
}

void PlayerManager::setState(PlayerState newState)
{
    if (mState == newState) return;
    mState = newState;
    mStateTimer = 0.0f;
    Animation* pAnim = getCurrentAnim();
    if (pAnim) pAnim->reset();
}

void PlayerManager::updateAnimation(double dt)
{
    Animation* pAnim = getCurrentAnim();
    if (pAnim) pAnim->update(dt);
}

Vector2D PlayerManager::getPosition() const
{
    if (mpPlayerUnit) return mpPlayerUnit->getPosition();
    return ZERO_VECTOR2D;
}

void PlayerManager::update(double dt)
{
    if (!mpPlayerUnit) return;

    if (mState == PlayerState::DEAD)
    {
        updateAnimation(dt);
        return;
    }
    if (mState == PlayerState::DYING)
    {
        mStateTimer += (float)dt;
        updateAnimation(dt);
        if (mStateTimer >= DYING_DURATION)
        {
            mDeathAnimDone = true;
            setState(PlayerState::DEAD);
        }
        return;
    }
    if (mState == PlayerState::SHOOTING_WEB)
    {
        mStateTimer += (float)dt;
        updateAnimation(dt);
        mpPlayerUnit->update(dt);
        if (mStateTimer >= SHOOT_DURATION)
            setState(mIsMoving ? PlayerState::WALKING : PlayerState::IDLE);
        return;
    }
    if (mState == PlayerState::WEB_BLAST)
    {
        mStateTimer += (float)dt;
        updateAnimation(dt);
        mpPlayerUnit->update(dt);
        if (mStateTimer >= BLAST_DURATION)
            setState(mIsMoving ? PlayerState::WALKING : PlayerState::IDLE);
        return;
    }
    if (mState == PlayerState::PUNCHING)
    {
        mStateTimer += (float)dt;
        updateAnimation(dt);

        // Keep punch hitbox in sync with player position
        Collider* pPunchCol = PhysicsManager::getInstance()->getCollider("playerPunch");
        if (pPunchCol)
            pPunchCol->setCenter(Vector2D(
                mpPlayerUnit->getPosition().getX() + PUNCH_HITBOX_OFFSET,
                mpPlayerUnit->getPosition().getY()));

        if (mStateTimer >= PUNCHING_DURATION)
            setState(mIsMoving ? PlayerState::WALKING : PlayerState::IDLE);
        return;
    }
    if (mState == PlayerState::BLOCKING) return;

    // Only IDLE / WALKING reach here
    Vector2D dir = mpPlayerUnit->getDirectionVector();
    bool actuallyMoving = (dir.getX() != 0.0f || dir.getY() != 0.0f);
    setState(actuallyMoving ? PlayerState::WALKING : PlayerState::IDLE);
    mpPlayerUnit->update(dt);
    updateAnimation(dt);
    mIsMoving = false;
}

void PlayerManager::draw(Vector2D cameraOffset)
{
    if (!mpPlayerUnit) return;

    Animation* pAnim = getCurrentAnim();
    if (pAnim && mAnimsLoaded)
    {
        Vector2D pos = mpPlayerUnit->getPosition();
        Vector2D screenPos(pos.getX() - cameraOffset.getX(),
            pos.getY() - cameraOffset.getY());
        GraphicsSystem::getBackBuffer()->draw(screenPos, pAnim->getCurrentSprite());
    }
    else
    {
        mpPlayerUnit->draw(cameraOffset);
    }
}

void PlayerManager::takeDamage(int amount)
{
    if (mState == PlayerState::DYING || mState == PlayerState::DEAD) return;

    float mult = 1.0f;
    if (mpWebManager && mpWebManager->isBlocking())
        mult = mpWebManager->getBlockDamageMultiplier();

    int finalDamage = (int)(amount * mult);
    if (finalDamage < 1 && amount > 0) finalDamage = 1;

    mHp -= finalDamage;
    if (mHp < 0) mHp = 0;

    std::cout << "Player took " << finalDamage << " (blocked: " << (mult < 1.0f ? "yes" : "no") << ") HP: " << mHp << "/" << mMaxHp << "\n";

    // Fire audio before setState so death sound triggers before state locks out events
    if (mpAudioManager)
    {
        if (mHp <= 0)
            mpAudioManager->playPlayerDeath();
        else
            mpAudioManager->playPlayerDamage();
    }

    if (mHp <= 0)
    {
        setState(PlayerState::DYING);
        if (mpPlayerUnit)
            mpPlayerUnit->setDirectionVector(Vector2D(0, 0));
    }
}

void PlayerManager::handleEvent(const Event& theEvent)
{
    if (mState == PlayerState::DYING || mState == PlayerState::DEAD) return;

    switch (theEvent.getType())
    {
    case MOVE_EVENT:
    {
        const MoveEvent& moveEvent = static_cast<const MoveEvent&>(theEvent);
        if (!moveEvent.isEnd())
        {
            mpPlayerUnit->addDirectionVector(moveEvent.getLocation());
            mIsMoving = true;
        }
        else
        {
            mpPlayerUnit->addDirectionVector(moveEvent.getLocation() * -1);
        }
        break;
    }

    case WEB_FIRE_EVENT:
        if (mState != PlayerState::SHOOTING_WEB)
            setState(PlayerState::SHOOTING_WEB);
        break;

    case WEB_BLAST_EVENT:
        if (mState != PlayerState::WEB_BLAST)
            setState(PlayerState::WEB_BLAST);
        break;

    case PUNCH_EVENT:
    {
        const PunchEvent& e = static_cast<const PunchEvent&>(theEvent);
        if (e.isPunching())
        {
            setState(PlayerState::PUNCHING);
            PhysicsManager::getInstance()->createCollider("playerPunch", PUNCH_COLLIDER_SIZE, 0.0f, Vector2D(mpPlayerUnit->getPosition().getX() + PUNCH_HITBOX_OFFSET, mpPlayerUnit->getPosition().getY()), true);
        }
        else
        {
            setState(mIsMoving ? PlayerState::WALKING : PlayerState::IDLE);
            PhysicsManager::getInstance()->deleteCollider("playerPunch");
        }
        break;
    }

    case BLOCK_EVENT:
    {
        const BlockEvent& e = static_cast<const BlockEvent&>(theEvent);
        if (e.isBlocking())
            setState(PlayerState::BLOCKING);
        else
            setState(mIsMoving ? PlayerState::WALKING : PlayerState::IDLE);
        break;
    }

    case MENU_DIFFICULTY_EVENT:
    {
        const MenuDifficultyEvent& e = static_cast<const MenuDifficultyEvent&>(theEvent);
        int d = e.getDifficulty();
        mMaxHp = (d == DIFFICULTY_EASY) ? HP_EASY : (d == DIFFICULTY_HARD) ? HP_HARD : HP_NORMAL;
        std::cout << "PlayerManager: difficulty " << d << " maxHp=" << mMaxHp << "\n";
        break;
    }

    case COLLISION_EVENT:
    {
        const CollisionEvent& col = static_cast<const CollisionEvent&>(theEvent);
        if (col.getCollider1Tag() != "player" && col.getCollider2Tag() != "player")
            return;
        if (col.getResolution() != ZERO_VECTOR2D)
            mpPlayerUnit->setPosition(getPosition().getX() + (col.getResolution().getX()), getPosition().getY() + (col.getResolution().getY()));
        break;
    }
    }
}

void PlayerManager::resetPlayer(Vector2D spawnPos)
{
    mHp = mMaxHp;
    mState = PlayerState::IDLE;
    mStateTimer = 0.0f;
    mIsMoving = false;
    mDeathAnimDone = false;

    for (auto& a : mIdleAnims) a.reset();
    for (auto& a : mWalkAnims) a.reset();
    for (auto& a : mShootAnims) a.reset();
    for (auto& a : mBlastAnims) a.reset();
    for (auto& a : mBlockAnims) a.reset();
    for (auto& a : mPunchingAnims) a.reset();
    for (auto& a : mDyingAnims) a.reset();

    if (mpPlayerUnit)
    {
        mpPlayerUnit->setPosition(spawnPos);
        mpPlayerUnit->setDirectionVector(Vector2D(0, 0));
    }
    std::cout << "PlayerManager: player reset, HP=" << mHp << "\n";
}

