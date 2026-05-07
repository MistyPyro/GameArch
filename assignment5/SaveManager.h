#pragma once
#include "Trackable.h"
#include <string>
#include <vector>
#include "Vector2D.h"

class PlayerManager;
class EnemyManager;
class WebManager;
class BossManager;
class WebProjectile;

struct PlayerSaveData
{
	float posX = 0.0f;
	float posY = 0.0f;
	int hp = 5;
	int maxHp = 5;
	float webMeter = 100.0f;
};

struct EnemySaveData
{
	float posX = 0.0f;
	float posY = 0.0f;
	int hp = 0;
	int state = 0;
};

struct WebProjectileSaveData
{
	float posX = 0.0f;
	float posY = 0.0f;
	float dirX = 0.0f;
	float dirY = 0.0f;
	float speed = 600.0f;
};

struct BossSaveData
{
	bool alive = false;
	float posX = 0.0f;
	float posY = 0.0f;
	int hp = 0;
	int maxHp = 30;
	int phase = 0;
};

struct LevelSaveData
{
	int currentLevel = 0;
	bool bossLevel = false;
};

struct GameSaveData
{
	bool valid = false;
	LevelSaveData level;
	PlayerSaveData player;
	BossSaveData boss;
	std::vector<EnemySaveData> enemies;
	std::vector<WebProjectileSaveData> webs;
};

class SaveManager : public Trackable
{
public:
	static SaveManager* getInstance();
	static void destroyInstance();
	bool saveGame(const std::string& filename, PlayerManager* pPlayer, EnemyManager* pEnemy, WebManager* pWeb, BossManager* pBoss, int currentLevel);
	bool hasSaveFile(const std::string& filename) const;
	bool loadGame(const std::string& filename, GameSaveData& outData);
	void deleteSave(const std::string& filename);

private:
	SaveManager(){}
	~SaveManager() {}
	static SaveManager* mpsInstance;
};