#pragma once
#include "EventListener.h"
#include <vector>
#include <string>
#include "Animation.h"
#include "Vector2D.h"
#include "Enemy.h"


class PlayerManager;
class FileManager;

class EnemyManager : public EventListener
{
    friend class FileManager;
public:
    static EnemyManager* createInstance(PlayerManager* pPlayerManager);
    static EnemyManager* getInstance() { return mpsInstance; }
    static void destroyInstance();
    void init();
    void cleanup();
    void setWalkAnimations(std::vector<Animation> anims) { mWalkAnims = anims; }
    void setAttackAnimations(std::vector<Animation> anims) { mAttackAnims = anims; }
    void setDyingAnimations(std::vector<Animation> anims) { mDyingAnims = anims; }
    void setStrongWalkAnimations(std::vector<Animation> anims) { mStrongWalkAnims = anims; }
    void setStrongAttackAnimations(std::vector<Animation> anims) { mStrongAttackAnims = anims; }
    void setStrongDyingAnimations(std::vector<Animation> anims) { mStrongDyingAnims = anims; }
    void spawnEnemy(Vector2D pos);
    void spawnStrongEnemy(Vector2D pos);
    void update(double dt);
    void draw(Vector2D cameraOffset = Vector2D(0, 0));
    unsigned int getNumEnemies() const { return (unsigned int)mEnemies.size(); }
    std::vector<Enemy*>& getEnemies() { return mEnemies; }
    void handleEvent(const Event& theEvent);
    void setCurrentLevel(int level) { mCurrentLevel = level; }
    void setEnemyHP(int hp) { ENEMY_HP = hp; mBaseEnemyHP = hp; }
    void setEnemyAttackDamage(int dmg) { ENEMY_ATTACK_DAMAGE = dmg; mBaseAttackDamage = dmg; }

private:
    static EnemyManager* mpsInstance;
    EnemyManager(PlayerManager* pPlayerManager);
    ~EnemyManager() { destroy(); }
    void destroy();
    //static int smNumEnemies;

    inline void setEnemySpeed(float speed) { ENEMY_SPEED = speed; };
  //  inline void setEnemyHP(int hp) { ENEMY_HP = hp; };
 //   inline void setEnemyAttackDamage(int damage) { ENEMY_ATTACK_DAMAGE = damage; };
    inline void setEnemyAttackRange(float range) { ENEMY_ATTACK_RANGE = range; };
    inline void setEnemyAttackCooldown(float cooldown) { ENEMY_ATTACK_COOLDOWN = cooldown; };
    inline void setStrongHPMultiplier(float mult) { STRONG_HP_MULT = mult; };
    inline void setStrongDamageMultiplier(float mult) { STRONG_DAMAGE_MULT = mult; };
    inline void setStrongSpeedMultiplier(float mult) { STRONG_SPEED_MULT = mult; };
    inline void setStrongRangeMultiplier(float mult) { STRONG_RANGE_MULT = mult; };

    
    std::vector<Enemy*>    mEnemies;
    PlayerManager* mpPlayerManager = nullptr;
    FileManager* mpFileManager = nullptr;
    std::vector<Animation> mWalkAnims;
    std::vector<Animation> mAttackAnims;
    std::vector<Animation> mDyingAnims;
    std::vector<Animation> mStrongWalkAnims;
    std::vector<Animation> mStrongAttackAnims;
    std::vector<Animation> mStrongDyingAnims;
    int mCurrentLevel = 0;
    int mBaseEnemyHP = 3;
    int mBaseAttackDamage = 1;

    const std::string DATA_FILE = "EnemyManagerData.txt";

    // THESE ARE DATA DRIVEN values here are fallback if file cannot be read
    float ENEMY_SPEED = 60.0f;
    int ENEMY_HP = 3;
    int ENEMY_ATTACK_DAMAGE = 1;
    float ENEMY_ATTACK_RANGE = 80.0f;
    float ENEMY_ATTACK_COOLDOWN = 800.0f;
    float STRONG_HP_MULT = 2.5f;
    float STRONG_DAMAGE_MULT = 2.0f;
    float STRONG_SPEED_MULT = 0.75f;
    float STRONG_RANGE_MULT = 1.2f;

    float ENEMY_COLLIDER_LENGTH = 60.0f;
    float ENEMY_COLLIDER_HEIGHT = 60.0f;
    float STRONG_ENEMY_COLLIDER_LENGTH = 100.0f;
    float STRONG_ENEMY_COLLIDER_HEIGHT = 100.0f;
};



