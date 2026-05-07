#pragma once
#include <Trackable.h>
#include <Vector2D.h>
#include <vector>
#include "Object.h"
#include "Animation.h"
#include "EventListener.h"

class GraphicsBuffer;

class Enemy : public EventListener, public Object
{
    friend class EnemyManager;
public:
    enum class EnemyType { NORMAL, STRONG };

    enum class EnemyState
    {
        WALKING,
        ATTACKING,
        DYING,
        DEAD
    };

    void update(double dt, Vector2D playerPos);
    void draw(Vector2D cameraOffset = Vector2D(0, 0));
    void takeDamage(int amount);
    bool isDead() const { return mState == EnemyState::DEAD; }
    bool isAttacking() const { return mState == EnemyState::ATTACKING; }
    Vector2D getPosition() const { return mPosition; }
    int getAttackDamage() const { return mAttackDamage; }
    EnemyState getState() const { return mState; }
    int getHp() const { return mHp; }
    int getMaxHp() const { return mMaxHp; }
    EnemyType getType() const { return mType; }
    void setHp(int hp) { mHp = hp; }
    void setPosition(Vector2D pos) { mPosition = pos; }

    void handleEvent(const Event& theEvent);

    bool didAttackLand();

private:
    Enemy(std::vector<Animation> walkAnim,
        std::vector<Animation> attackAnim,
        std::vector<Animation> dyingAnim,
        Vector2D startPos,
        float moveSpeed,
        int maxHp,
        int attackDamage,
        float attackRange,
        float attackCooldownMs,
        EnemyType type = EnemyType::NORMAL);
    ~Enemy() {}

    void setState(EnemyState newState);
    Animation* getCurrentAnim();
    std::vector<Animation> mWalkAnims;
    std::vector<Animation> mAttackAnims;
    std::vector<Animation> mDyingAnims;
    EnemyType mType = EnemyType::NORMAL;
    EnemyState mState = EnemyState::WALKING;
    Vector2D mPosition;
    float mMoveSpeed = 0.0f;
    int mHp = 0;
    int mMaxHp = 0;
    int mAttackDamage = 0;
    float mAttackRange = 0.0f;
    float mAttackCooldownMs = 0.0f;
    float mAttackCooldownTimer = 0.0f;
    float mDyingTimer = 0.0f;
    const float DYING_DURATION_MS = 800.0f;
    bool mAttackLanded = false;
    float mDrawScale = 1.0f;
    int mID = 0;
    static constexpr int PUNCH_DAMAGE = 1;
};


