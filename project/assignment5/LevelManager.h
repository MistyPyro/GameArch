#pragma once
#include "Trackable.h"
#include "Vector2D.h"
#include <string>

class PlayerManager;
class BossManager;
class FileManager;

class LevelManager : public Trackable
{
	friend class FileManager;
public: 
	static LevelManager* createInstance();
	static LevelManager* getInstance() { return mpsInstance; };
	static void destroyInstance();

	void init(float worldWidth, float screenWidth, bool hasBoss, int totalLevels);
	bool update(PlayerManager* pPlayer, BossManager* pBoss);
	bool isLevelComplete() const { return mLevelComplete; }
	bool isFinalLevel() const { return mCurrentLevel >= mTotalLevels - 1; }
	int getCurrentLevel() const { return mCurrentLevel; }
	int getTotalLevels() const { return mTotalLevels; }
	void advanceLevel();
	void resetLevel();
	void setCurrentLevel(int level) { mCurrentLevel = level; }
	void setHasBoss(bool b) { mHasBoss = b; }

private:
	static LevelManager* mpsInstance;
	LevelManager() {};
	~LevelManager() {};

	float mWorldWidth = 3200.0f;
	float mScreenWidth = 900.0f;
	float mEndZoneX = 0.0f;
	bool mHasBoss = false;
	bool mLevelComplete = false;
	int mCurrentLevel = 0;
	int mTotalLevels = 3;

	inline void setEndZoneMargin(float margin) { END_ZONE_MARGIN = margin; };

	const std::string DATA_FILE = "LevelManagerData.txt";

	// THESE ARE DATA DRIVEN values here are fallback if file cannot be read
	float END_ZONE_MARGIN = 200.0f;
};