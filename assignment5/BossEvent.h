#pragma once
#include "GameEvent.h"

class BossPhaseEvent : public GameEvent
{
public:
    BossPhaseEvent(int phase) : GameEvent(BOSS_PHASE_EVENT), mPhase(phase) {}
    ~BossPhaseEvent() {}
    int getPhase() const { return mPhase; }
private:
    int mPhase;
};

class BossDeadEvent : public GameEvent
{
public:
    BossDeadEvent() : GameEvent(BOSS_DEAD_EVENT) {}
    ~BossDeadEvent() {}
};

class BossAttackEvent : public GameEvent
{
public:
    BossAttackEvent(int attackType) : GameEvent(BOSS_ATTACK_EVENT), mAttackType(attackType) {}
    ~BossAttackEvent() {}
    int getAttackType() const { return mAttackType; }
private:
    int mAttackType;
};
