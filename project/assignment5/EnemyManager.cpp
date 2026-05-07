#include "EnemyManager.h"
#include "../Enemy.h"
#include "../PlayerManager.h"
#include "EventSystem.h"
#include "../GameEvent.h"
#include "../ConsumingEvent.h"
#include "../SpawnEvent.h"
#include "../ScoreEvent.h"
#include "../MenuEvent.h"
#include "../Unit.h"
#include <iostream>
#include "FileManager.h"
#include "PhysicsManager.h"
#include "Collider.h"

EnemyManager* EnemyManager::mpsInstance = nullptr;
//int EnemyManager::smNumEnemies = 0;

EnemyManager* EnemyManager::createInstance(PlayerManager* pPlayerManager)
{
    if (!mpsInstance)
        mpsInstance = new EnemyManager(pPlayerManager);
    return mpsInstance;
}

void EnemyManager::destroyInstance()
{
    if (mpsInstance)
    {
        mpsInstance->destroy(); 
        delete mpsInstance;
        mpsInstance = nullptr;
    }
}

EnemyManager::EnemyManager(PlayerManager* pPlayerManager) : mpPlayerManager(pPlayerManager)
{
    EventSystem::getInstance()->addListener((EventType)SPAWN_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_DIFFICULTY_EVENT, this);
}

void EnemyManager::init()
{
    FileManager::getInstance()->readAndImportFile(DATA_FILE);
    // Stats are set from GameData.txt via FileManager friend access
}

void EnemyManager::cleanup()
{
    for (Enemy* pEnemy : mEnemies)
    {
        EventSystem::getInstance()->removeListenerFromAllEvents(pEnemy);
        std::string key = "enemy" + std::to_string((uintptr_t)pEnemy);
        PhysicsManager::getInstance()->deleteCollider(key);
        delete pEnemy;
    }

    mEnemies.clear();
}

void EnemyManager::destroy()
{
    EventSystem::getInstance()->removeListenerFromAllEvents(this);
    for (Enemy* pEnemy : mEnemies)
    {
        EventSystem::getInstance()->removeListenerFromAllEvents(pEnemy);
        std::string key = "enemy" + std::to_string((uintptr_t)pEnemy);
        if (PhysicsManager::getInstance())
            PhysicsManager::getInstance()->deleteCollider(key);
        delete pEnemy;
    }
    mEnemies.clear();
    mpPlayerManager = nullptr;

}

void EnemyManager::spawnEnemy(Vector2D pos)
{
    if (mWalkAnims.empty()) return;

    Enemy* pEnemy = new Enemy(mWalkAnims, mAttackAnims, mDyingAnims, pos, ENEMY_SPEED, ENEMY_HP, ENEMY_ATTACK_DAMAGE, ENEMY_ATTACK_RANGE, ENEMY_ATTACK_COOLDOWN, Enemy::EnemyType::NORMAL);

    mEnemies.push_back(pEnemy);
    EventSystem::getInstance()->addListener((EventType)COLLISION_EVENT, pEnemy);
    std::string key = "enemy" + std::to_string((uintptr_t)pEnemy);
    PhysicsManager::getInstance()->createCollider(key, 60.0f, 60.0f, pos, true, pEnemy);

    std::cout << "EnemyManager: spawned normal enemy at "
        << pos.getX() << "," << pos.getY() << "\n";
}

void EnemyManager::spawnStrongEnemy(Vector2D pos)
{
    if (mWalkAnims.empty()) return;

    auto& walk = mStrongWalkAnims.empty() ? mWalkAnims : mStrongWalkAnims;
    auto& attack = mStrongAttackAnims.empty() ? mAttackAnims : mStrongAttackAnims;
    auto& dying = mStrongDyingAnims.empty() ? mDyingAnims : mStrongDyingAnims;

    int hp = (int)(ENEMY_HP * STRONG_HP_MULT);
    int dmg = (int)(ENEMY_ATTACK_DAMAGE * STRONG_DAMAGE_MULT);
    float speed = ENEMY_SPEED * STRONG_SPEED_MULT;
    float range = ENEMY_ATTACK_RANGE * STRONG_RANGE_MULT;
    float cd = ENEMY_ATTACK_COOLDOWN;

    Enemy* pEnemy = new Enemy(walk, attack, dying, pos, speed, hp, dmg, range, cd, Enemy::EnemyType::STRONG);

    mEnemies.push_back(pEnemy);
    EventSystem::getInstance()->addListener((EventType)COLLISION_EVENT, pEnemy);
    std::string key = "enemy" + std::to_string((uintptr_t)pEnemy);
    PhysicsManager::getInstance()->createCollider(key, 80.0f, 80.0f, pos, true, pEnemy);

    std::cout << "EnemyManager: spawned STRONG enemy HP=" << hp << " DMG=" << dmg << " at " << pos.getX() << "," << pos.getY() << "\n";

}

void EnemyManager::update(double dt)
{
    if (!mpPlayerManager || !mpPlayerManager->getPlayer()) return;

    Vector2D playerPos = mpPlayerManager->getPlayer()->getPosition();

    for (int i = (int)mEnemies.size() - 1; i >= 0; i--)
    {
        Enemy* pEnemy = mEnemies[i];
        pEnemy->update(dt, playerPos);

        if (pEnemy->didAttackLand())
        {
            mpPlayerManager->takeDamage(pEnemy->getAttackDamage());
            std::cout << "Enemy hit player for " << pEnemy->getAttackDamage() << " damage!\n";
            EventSystem::getInstance()->fireEvent(ConsumingEvent(0.0f, "destroy"));
        }

        if (pEnemy->isDead())
        {
            // Strong enemies worth more score
            int score = (pEnemy->getType() == Enemy::EnemyType::STRONG) ? 25 : 10;
            EventSystem::getInstance()->fireEvent(ScoreEvent(score));
         //   EventSystem::getInstance()->fireEvent(ConsumingEvent(0.0f, "consuming"));
            EventSystem::getInstance()->removeListenerFromAllEvents(pEnemy);
            std::string key = "enemy" + std::to_string((uintptr_t)pEnemy);
            PhysicsManager::getInstance()->deleteCollider(key);
            delete pEnemy;
            mEnemies.erase(mEnemies.begin() + i);
        }
    }
}

void EnemyManager::draw(Vector2D cameraOffset)
{
    for (Enemy* pEnemy : mEnemies)
        pEnemy->draw(cameraOffset);
}

void EnemyManager::handleEvent(const Event& theEvent)
{
    switch (theEvent.getType())
    {
    case SPAWN_EVENT:
    {
        const SpawnEvent& spawnEvent = static_cast<const SpawnEvent&>(theEvent);
        spawnEnemy(spawnEvent.getLocation());
        break;
    }

    case MENU_DIFFICULTY_EVENT:
    {
        const MenuDifficultyEvent& e = static_cast<const MenuDifficultyEvent&>(theEvent);
        int d = e.getDifficulty();
        float hpMult = (d == 0) ? 0.7f : (d == 2) ? 1.5f : 1.0f;
        float damageMult = (d == 0) ? 0.7f : (d == 2) ? 1.5f : 1.0f;
        ENEMY_HP = (int)(mBaseEnemyHP * hpMult);
        ENEMY_ATTACK_DAMAGE = (int)(mBaseAttackDamage * damageMult);
        ENEMY_ATTACK_COOLDOWN = (d == 0) ? 1200.0f : (d == 2) ? 600.0f : 800.0f;
        break;
    }
    default:
        break;
    }
}
