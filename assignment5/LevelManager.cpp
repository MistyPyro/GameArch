#include "LevelManager.h"
#include "PlayerManager.h"
#include "BossManager.h"
#include <iostream>
#include "FileManager.h"

LevelManager* LevelManager::mpsInstance = nullptr;

LevelManager* LevelManager::createInstance()
{
    if (mpsInstance == nullptr)
        mpsInstance = new LevelManager;
    return mpsInstance;
}

void  LevelManager::destroyInstance()
{
    delete mpsInstance;
    mpsInstance = nullptr;
}

void LevelManager::init(float worldWidth, float screenWidth,
    bool hasBoss, int totalLevels)
{
    FileManager::getInstance()->readAndImportFile(DATA_FILE);
    mWorldWidth = worldWidth;
    mScreenWidth = screenWidth;
    mHasBoss = hasBoss;
    mTotalLevels = totalLevels;
    mLevelComplete = false;
    mEndZoneX = mWorldWidth - END_ZONE_MARGIN;
}

bool LevelManager::update(PlayerManager* pPlayer, BossManager* pBoss)
{
    if (mLevelComplete) return true;
    if (!pPlayer || pPlayer->isDead()) return false;

    float playerX = pPlayer->getPosition().getX();

    if (mHasBoss)
    {
        if (pBoss && pBoss->isBossDefeated())
        {
            mLevelComplete = true;
            std::cout << "LevelManager: boss defeated — level "
                << mCurrentLevel << " complete!\n";
            return true;
        }
    }
    else
    {
        if (playerX >= mEndZoneX)
        {
            mLevelComplete = true;
            std::cout << "LevelManager: player reached end — level "
                << mCurrentLevel << " complete!\n";
            return true;
        }
    }
    return false;
}

void LevelManager::advanceLevel()
{
    mCurrentLevel++;
    mLevelComplete = false;
    std::cout << "LevelManager: advancing to level " << mCurrentLevel << "\n";
}

void LevelManager::resetLevel()
{
    mLevelComplete = false;
    std::cout << "LevelManager: level " << mCurrentLevel << " reset\n";
}
