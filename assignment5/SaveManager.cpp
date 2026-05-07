#include "SaveManager.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "WebManager.h"
#include "WebProjectile.h"
#include "BossManager.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>

SaveManager* SaveManager::mpsInstance = nullptr;

SaveManager* SaveManager::getInstance()
{
    if (!mpsInstance)
        mpsInstance = new SaveManager();
    return mpsInstance;
}

void SaveManager::destroyInstance()
{
    delete mpsInstance;
    mpsInstance = nullptr;
}

bool SaveManager::hasSaveFile(const std::string& filename) const
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

void SaveManager::deleteSave(const std::string& filename)
{
    std::remove(filename.c_str());
    std::cout << "SaveManager: deleted " << filename << "\n";
}


bool SaveManager::saveGame(const std::string& filename, PlayerManager* pPlayer, EnemyManager* pEnemy, WebManager* pWeb, BossManager* pBoss, int currentLevel)
{
    std::cout << "SaveManager: writing to absolute path check: " << filename << "\n";

    std::ofstream f(filename);
    if (!f.is_open())
    {
        std::cout << "SaveManager: cannot open " << filename << " for writing\n";
        return false;
    }

    f << "#save\n";
    f << currentLevel << "\n";

    // Player
    f << "#player\n";
    if (pPlayer)
    {
        Vector2D pos = pPlayer->getPosition();
        f << pos.getX() << " " << pos.getY() << " " << pPlayer->getHp() << " " << pPlayer->getMaxHp() << " " << (pWeb ? pWeb->getMeterFraction() * 100.0f : 100.0f) << "\n";
    }
    else
    {
        f << "0 0 5 5 100\n";
    }

    // Enemies
    f << "#enemies\n";
    if (pEnemy)
    {
        auto& enemies = pEnemy->getEnemies();
        std::vector<Enemy*> live;
        for (Enemy* e : enemies)
            if (!e->isDead()) live.push_back(e);

        f << live.size() << "\n";
        for (Enemy* e : live)
        {
            f << e->getPosition().getX() << " " << e->getPosition().getY() << " " << e->getHp() << " " << (int)e->getState() << "\n";
        }
    }
    else
    {
        f << "0\n";
    }




    f << "#webs\n";
    if (pWeb)
    {
        const WebProjectilePool& pool = pWeb->getPool();
        int count = 0;
        for (int i = 0; i < pool.size(); i++)
            if (pool.getAll()[i]->isActive()) count++;

        f << count << "\n";
        for (int i = 0; i < pool.size(); i++)
        {
            WebProjectile* p = pool.getAll()[i];
            if (!p->isActive()) continue;
            f << p->getPosition().getX() << " " << p->getPosition().getY() << " "
                << p->getDirection().getX() << " " << p->getDirection().getY() << " "
                << p->getSpeed() << "\n";
        }
    }
    else
    {
        f << "0\n";  
    }

    // Boss
    f << "#boss\n";
    if (pBoss && pBoss->isBossAlive())
    {
        Vector2D bp = pBoss->getBossPosition();
        f << "1 " << bp.getX() << " " << bp.getY() << " " << pBoss->getBossHp() << " " << pBoss->getBossMaxHp() << " " << pBoss->getBossPhase() << "\n";
    }
    else
    {
        f << "0 0 0 0 0 0\n";
    }

    std::cout << "SaveManager: game saved to " << filename << "\n";
    return true;
}

bool SaveManager::loadGame(const std::string& filename, GameSaveData& outData)
{
    std::ifstream f(filename);
    if (!f.is_open())
    {
        std::cout << "SaveManager: cannot open " << filename << "\n";
        return false;
    }

    std::string tag;
    outData = GameSaveData();   

    // #save
    if (!(f >> tag) || tag != "#save") return false;
    f >> outData.level.currentLevel;

    // #player
    if (!(f >> tag) || tag != "#player") return false;
    f >> outData.player.posX >> outData.player.posY >> outData.player.hp >> outData.player.maxHp >> outData.player.webMeter;

    // #enemies
    if (!(f >> tag) || tag != "#enemies") return false;
    int enemyCount = 0;
    f >> enemyCount;
    outData.enemies.resize(enemyCount);
    for (int i = 0; i < enemyCount; i++)
    {
        f >> outData.enemies[i].posX >> outData.enemies[i].posY >> outData.enemies[i].hp >> outData.enemies[i].state;
    }

    // #webs
    if (!(f >> tag) || tag != "#webs") return false;
    int webCount = 0;
    f >> webCount;
    outData.webs.resize(webCount);
    for (int i = 0; i < webCount; i++)
    {
        f >> outData.webs[i].posX >> outData.webs[i].posY >> outData.webs[i].dirX >> outData.webs[i].dirY >> outData.webs[i].speed;
    }

    // #boss
    if (!(f >> tag) || tag != "#boss") return false;
    int bossAlive = 0;
    f >> bossAlive >> outData.boss.posX >> outData.boss.posY >> outData.boss.hp >> outData.boss.maxHp >> outData.boss.phase;
    outData.boss.alive = (bossAlive == 1);

    outData.valid = true;
    std::cout << "SaveManager: loaded from " << filename
        << " (level " << outData.level.currentLevel << ")\n";
    return true;
}
