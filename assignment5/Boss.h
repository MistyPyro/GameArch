#pragma once
#include <Trackable.h>
#include <Vector2D.h>
#include <vector>
#include "Object.h"
#include "Animation.h"

class PlayerManager;

class Boss : public Trackable, public Object
{
    friend class BossManager;
public:
    enum class BossAttack
    {
        NONE,
        BIG_SWING,          
        TONGUE_ATTACK,      
        LARGE_FORWARD,      
        SIMPLE_JAB,         
        SHARK_CHARGE,       
        BIG_PUNCH,          
        DOWNWARD_ATTACK,    
        DOWN_SPIKE,         
        CHARGE_FORWARD,     
        TENTACLE_ATTACK     
    };

    enum class BossState
    {
        IDLE,
        WALKING,
        ATTACKING,
        HURT,
        DYING,
        DEAD
    };

    enum class BossPhase { ONE, TWO, THREE };

    void update(double dt, Vector2D playerPos);
    void draw(Vector2D cameraOffset);
    void takeDamage(int amount);
    bool isDead() const { return mState == BossState::DEAD; }
    bool isActive() const { return mState != BossState::DEAD; }
    Vector2D getPosition() const { return mPosition; }
    int  getHp() const { return mHp; }
    int  getMaxHp() const { return mMaxHp; }
    BossPhase getPhase() const { return mPhase; }
    BossAttack getCurrentAttack() const { return mCurrentAttack; }
    bool isHitboxActive() const { return mHitboxActive; }
    float getAttackRange() const;
    int   getAttackDamage() const;

private:
    Boss(std::vector<Animation> idleAnim, std::vector<Animation> walkAnim, std::vector<Animation> attackAnims, std::vector<Animation> hurtAnim, std::vector<Animation> dyingAnim, Vector2D startPos, int maxHp);
    ~Boss() {}
    void setState(BossState newState);
    void setAttack(BossAttack attack);
    void chooseNextAttack(Vector2D playerPos);
    void updatePhase();
    Animation* getCurrentAnim();
    std::vector<Animation> mIdleAnims;
    std::vector<Animation> mWalkAnims;
    std::vector<Animation> mAttackAnims; 
    std::vector<Animation> mHurtAnims;
    std::vector<Animation> mDyingAnims;
    BossState  mState = BossState::IDLE;
    BossAttack mCurrentAttack = BossAttack::NONE;
    BossPhase  mPhase = BossPhase::ONE;
    Vector2D mPosition;
    int mHp;
    int mMaxHp;
    float mMoveSpeed = 80.0f;

    float mAttackTimer = 0.0f;   
    float mAttackDuration = 0.0f;
    float mHitboxTimer = 0.0f;   
    bool mHitboxActive = false;
    bool mHitDealtThisSwing = false; 
    float mCooldownTimer = 0.0f; 
    float mHurtTimer = 0.0f;
    float mDyingTimer = 0.0f;
    static constexpr float DYING_DURATION = 1800.0f;
    float attackDuration(BossAttack a) const;
    float hitboxStart(BossAttack a) const;
    float hitboxEnd(BossAttack a) const;
    float cooldownAfter(BossAttack a) const;
    static constexpr int BOSS_KILL_SCORE = 500;
};
