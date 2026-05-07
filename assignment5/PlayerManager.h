#pragma once
#include "EventListener.h"
#include "PhysicsManager.h"
#include "Vector2D.h"
#include "Animation.h"
#include "Object.h"
#include <vector>
#include <string>

class Unit;
class WebManager;
class PhysicsManager;
class GameAudioManager;

class PlayerManager : public EventListener, public Object
{
    friend class FileManager;
public:
    enum class PlayerState
    {
        IDLE,
        WALKING,
        SHOOTING_WEB, 
        WEB_BLAST,     
        BLOCKING,      
        PUNCHING,
        DYING,
        DEAD
    };

    static PlayerManager* createInstance();
    static PlayerManager* getInstance() { return mpsInstance; };
    static void destroyInstance();

    void init();
    void cleanup();
    void setAnimations(std::vector<Animation> idle,
        std::vector<Animation> walk,
        std::vector<Animation> shootWeb,
        std::vector<Animation> webBlast,
        std::vector<Animation> block,
        std::vector<Animation> punching,
        std::vector<Animation> dying);

    void createPlayer(std::vector<Animation> fallbackAnims,
        Vector2D pos, float moveSpeed);

    Unit* getPlayer() { return mpPlayerUnit; }
    bool  isDead() const { return mState == PlayerState::DEAD; }
    bool  isDeathAnimDone() const { return mDeathAnimDone; }

    void update(double dt);
    void draw(Vector2D cameraOffset = Vector2D(0, 0));

    // Health
    void takeDamage(int amount);
    int  getHp() const { return mHp; }
    int  getMaxHp() const { return mMaxHp; }
    void setAudioManager(GameAudioManager* p) { mpAudioManager = p; }

    void setWebManager(WebManager* pWebManager) { mpWebManager = pWebManager; }

    void handleEvent(const Event& theEvent);

    Vector2D getPosition() const override;

    void resetPlayer(Vector2D spawnPos);

    void setHp(int hp) { mHp = hp < 0 ? 0 : hp > mMaxHp ? mMaxHp : hp; }

private:
    static PlayerManager* mpsInstance;
    PlayerManager();
    ~PlayerManager() { cleanup(); }

    void setState(PlayerState newState);
    void updateAnimation(double dt);
    Animation* getCurrentAnim();

    Unit* mpPlayerUnit = nullptr;
    WebManager* mpWebManager = nullptr;
    GameAudioManager* mpAudioManager = nullptr;

    PlayerState mState = PlayerState::IDLE;
    float mStateTimer = 0.0f;   

    std::vector<Animation> mIdleAnims;
    std::vector<Animation> mWalkAnims;
    std::vector<Animation> mShootAnims;
    std::vector<Animation> mBlastAnims;
    std::vector<Animation> mBlockAnims;
    std::vector<Animation> mDyingAnims;
    std::vector<Animation> mPunchingAnims;

    bool mAnimsLoaded = false;
    bool mDeathAnimDone = false;
    int mHp = 5;
    int mMaxHp = 5;
    bool mIsMoving = false;

    const std::string DATA_FILE = "PlayerManagerData.txt";

    // THESE ARE DATA DRIVEN - CHANGE FILE VALUES NOT THESE | values here are fallback if file cannot be read
    float SHOOT_DURATION = 400.0f;
    float BLAST_DURATION = 500.0f;
    float PUNCHING_DURATION = 600.0f;
    float DYING_DURATION = 1200.0f;
    float PLAYER_COLLIDER_SIZE = 50.0f; 
    float PUNCH_COLLIDER_SIZE = 30.0f;  
    float PUNCH_HITBOX_OFFSET = 40.0f;  
    float WALL_PUSH_FORCE = 9.0f;
    int HP_EASY = 7;
    int HP_NORMAL = 5;
    int HP_HARD = 3;
    int DIFFICULTY_EASY = 0;
    int DIFFICULTY_NORMAL = 1;
    int DIFFICULTY_HARD = 2;

    inline void setShootDuration(float dur) { SHOOT_DURATION = dur; };
    inline void setBlastDuration(float dur) { BLAST_DURATION = dur; };
    inline void setPunchingDuration(float dur) { PUNCHING_DURATION = dur; };
    inline void setDyingDuration(float dur) { DYING_DURATION = dur; };
    inline void setPlayerColliderSize(float size) { PLAYER_COLLIDER_SIZE = size; };
    inline void setPunchColliderSize(float size) { PUNCH_COLLIDER_SIZE = size; };
    inline void setPunchHitboxOffset(float offset) { PUNCH_HITBOX_OFFSET = offset; };
    inline void setWallPushForce(float force) { WALL_PUSH_FORCE = force; };
    inline void setHPEasy(int hp) { HP_EASY = hp; };
    inline void setHPNormal(int hp) { HP_NORMAL = hp; };
    inline void setHPHard(int hp) { HP_HARD = hp; };
    inline void setDifficultyEasy(int difficulty) { DIFFICULTY_EASY = difficulty; };
    inline void setDifficultyNormal(int difficulty) { DIFFICULTY_NORMAL = difficulty; };
    inline void setDifficultyHard(int difficulty) { DIFFICULTY_HARD = difficulty; };

};


