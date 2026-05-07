#pragma once
#include "EventListener.h"
#include "Vector2D.h"
#include "Animation.h"
#include <vector>
#include <string>
#include "Boss.h"

class PlayerManager;
class Camera;
class GraphicsBuffer;
class Font;
class GraphicsBufferManager;
class FileManager;

class BossManager : public EventListener
{
    friend class FileManager;
public:
    static BossManager* createInstance(PlayerManager* pPlayerManager, Camera* pCamera);
    static BossManager* getInstance() { return mpsInstance; };
    static void destroyInstance();

    void init(Font* pFont);
    void cleanup();

    void loadBossAnimations(
        GraphicsBuffer* pIdleBuf, unsigned int idleW, unsigned int idleH, unsigned int idleCols, unsigned int idleRows,
        GraphicsBuffer* pWalkBuf, unsigned int walkW, unsigned int walkH, unsigned int walkCols, unsigned int walkRows,
        GraphicsBuffer* pHurtBuf, unsigned int hurtW, unsigned int hurtH, unsigned int hurtCols, unsigned int hurtRows,
        GraphicsBuffer* pDyingBuf, unsigned int dyingW, unsigned int dyingH, unsigned int dyingCols, unsigned int dyingRows
    );

    void loadBossAnimationsFromFiles(
        const std::vector<std::string>& idleFiles, unsigned int idleW, unsigned int idleH,
        const std::vector<std::string>& walkFiles, unsigned int walkW, unsigned int walkH,
        const std::vector<std::string>& hurtFiles, unsigned int hurtW, unsigned int hurtH,
        const std::vector<std::string>& dyingFiles, unsigned int dyingW, unsigned int dyingH
    );

    void addAttackAnimationFromFiles(const std::vector<std::string>& files, unsigned int frameW, unsigned int frameH, bool loops = false);

    void setGraphicsBufferManager(GraphicsBufferManager* p) { mpGraphicsBufferManager = p; }
    void addAttackAnimation(GraphicsBuffer* pBuf, unsigned int frameW, unsigned int frameH, unsigned int cols, unsigned int rows, bool loops = false);
    void spawnBoss(Vector2D pos, int maxHp = 30);
    void update(double dt);
    void draw();
    void drawHUD(); 
    bool isBossAlive() const;
    bool isBossSpawned() const { return mpBoss != nullptr; }
    void handleEvent(const Event& theEvent);
    Vector2D getBossPosition() const;
    void damageBoss(int amount);
    int getBossHp() const { return mpBoss ? mpBoss->getHp() : 0; }
    int getBossMaxHp() const { return mpBoss ? mpBoss->getMaxHp() : 0; }
    int getBossPhase() const { return mpBoss ? (int)mpBoss->getPhase() : 0; }
    Vector2D getBossCenter() const
    {
        if (!mpBoss) return Vector2D(0, 0);
        return Vector2D(mpBoss->getPosition().getX() + 128.0f,
            mpBoss->getPosition().getY() + 128.0f);
    }
    bool wasBossSpawned() const { return mBossWasSpawned; }
    bool isBossDefeated() const { return mBossWasSpawned && mpBoss == nullptr; }
    void clearBoss()
    {
        delete mpBoss;
        mpBoss = nullptr;
        mBossWasSpawned = false;
    }

private:
    static BossManager* mpsInstance;
    BossManager(PlayerManager* pPlayerManager, Camera* pCamera);
    ~BossManager() { cleanup(); }

    std::vector<Animation> buildAnims(GraphicsBuffer* pBuf, unsigned int frameW, unsigned int frameH, unsigned int cols, unsigned int rows, bool loops);
    void drawBossHealthBar();
    Boss* mpBoss = nullptr;
    PlayerManager* mpPlayerManager = nullptr;
    Camera* mpCamera = nullptr;
    Font* mpFont = nullptr;
    std::vector<Animation> mIdleAnims;
    std::vector<Animation> mWalkAnims;
    std::vector<Animation> mAttackAnims;
    std::vector<Animation> mHurtAnims;
    std::vector<Animation> mDyingAnims;
    GraphicsBufferManager* mpGraphicsBufferManager = nullptr;
    std::vector<Animation> buildAnimsFromFiles(const std::vector<std::string>& filePaths, unsigned int frameW, unsigned int frameH, bool loops);

    inline void setBarX(float x) { BAR_X = x; };
    inline void setBarY(float y) { BAR_Y = y; };
    inline void setBarW(float w) { BAR_W = w; };
    inline void setBarH(float h) { BAR_H = h; };

    const std::string DATA_FILE = "BossManagerData.txt";
    bool mBossWasSpawned = false;

    // THESE ARE DATA DRIVEN values here are fallback if file cannot be read
    float BAR_X = 100.0f;
    float BAR_Y = 10.0f;
    float BAR_W = 300.0f;
    float BAR_H = 22.0f;
    float BAR_PAD = 2.0f;
};
