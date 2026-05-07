#pragma once
#include "EventListener.h"
#include "Vector2D.h"
#include "Color.h"
#include "../WebProjectile.h" 
#include <vector>
#include <string>

class EnemyManager;
class BossManager;
class PlayerManager;
class Font;
class FileManager;

class WebManager : public EventListener
{
    friend class FileManager;
public:
    static WebManager* createInstance(EnemyManager* pEnemyManager);
    static WebManager* getInstance() { return mpsInstance; };
    static void destroyInstance();

    void init(Font* pFont);
    void cleanup();
    void update(double dt);
    void draw(Vector2D cameraOffset = Vector2D(0, 0));
    void drawHUD();
    float getMeterFraction() const { return mWebMeter / mWebMeterMax; }
    bool  isBlocking() const { return mBlocking; }
    float getBlockDamageMultiplier() const
    {
        return mBlocking ? BLOCK_DAMAGE_REDUCTION : 1.0f;
    }

    void setBossManager(BossManager* p) { mpBossManager = p; }
    void setPlayerManager(PlayerManager* p) { mpPlayerManager = p; }
    WebProjectilePool& getPool() { return mPool; }
    void setDifficulty(int difficulty);
    void handleEvent(const Event& theEvent);

private:
    static WebManager* mpsInstance;
    WebManager(EnemyManager* pEnemyManager);
    ~WebManager() { destroy(); }
    void destroy();


    void fireWeb(Vector2D origin, Vector2D target);
    void checkWebCollisions();
    void fireBlast(Vector2D origin, Vector2D target);
    void updateBlast(double dt);
    void drawBlast();
    void checkBlastCollisions();
    void drawMeter();
    void tryConsumeWeb(float cost);
    Vector2D getPlayerOrigin() const;
    WebProjectilePool mPool;
    bool mBlastActive = false;
    float mBlastTimer = 0.0f;
    Vector2D mBlastOrigin;
    Vector2D mBlastDirection;
    float mBlastWidth = 0.0f;
    bool mBlastHitThisFrame = false;
    bool mBlocking = false;
    float mWebMeter = 100.0f;
    const float mWebMeterMax = 100.0f;
    bool mMeterWasEmpty = false;
    float mRegenDelayTimer = 0.0f;
    float mRegenRate = 8.0f;
    EnemyManager* mpEnemyManager = nullptr;
    BossManager* mpBossManager = nullptr;
    PlayerManager* mpPlayerManager = nullptr;
    Font* mpFont = nullptr;

    inline void setWebCost(float cost) { WEB_COST = cost; };
    inline void setBlastCost(float cost) { BLAST_COST = cost; };
    inline void setRegenDelay(float ms) { REGEN_DELAY_MS = ms; };
    inline void setWebSpeed(float speed) { WEB_SPEED = speed; };
    inline void setHitRadius(float radius) { HIT_RADIUS = radius; };
    inline void setWebDamage(int damage) { WEB_DAMAGE = damage; };
    inline void setBlastDamage(int damage) { BLAST_DAMAGE = damage; };
    inline void setBlastDuration(float ms) { BLAST_DURATION_MS = ms; };
    inline void setBlastMaxWidth(float width) { BLAST_MAX_WIDTH = width; };
    inline void setBlastHitRadius(float radius) { BLAST_HIT_RADIUS = radius; };
    inline void setBlockDamageReduction(float mult) { BLOCK_DAMAGE_REDUCTION = mult; };
    inline void setBlockSpeedMultiplier(float mult) { BLOCK_SPEED_MULT = mult; };
    inline void setBarX(float x) { BAR_X = x; };
    inline void setBarY(float y) { BAR_Y = y; };
    inline void setBarW(float width) { BAR_W = width; };
    inline void setBarH(float height) { BAR_H = height; };
    inline void setBarPadding(float padding) { BAR_PADDING = padding; };
    inline void setBGColor(Color color) { COL_BG = color; };
    inline void setFillColor(Color color) { COL_FILL = color; };
    inline void setBlastFillColor(Color color) { COL_BLAST_FILL = color; };
    inline void setFrameColor(Color color) { COL_FRAME = color; };
    inline void setTextColor(Color color) { COL_TEXT = color; };
    inline void setEmptyColor(Color color) { COL_EMPTY = color; };
    inline void setBlockColor(Color color) { COL_BLOCK = color; };

    const std::string DATA_FILE = "WebManagerData.txt";

    // THESE ARE DATA DRIVEN - CHANGE FILE VALUES NOT THESE | values here are fallback if file cannot be read
    float WEB_COST = 20.0f;
    float BLAST_COST = 60.0f;
    float REGEN_DELAY_MS = 1500.0f;
    float WEB_SPEED = 600.0f;
    float HIT_RADIUS = 50.0f;
    int WEB_DAMAGE = 1;
    int BLAST_DAMAGE = 3;
    float BLAST_DURATION_MS = 350.0f;
    float BLAST_MAX_WIDTH = 28.0f;
    float BLAST_HIT_RADIUS = 60.0f;
    float BLOCK_DAMAGE_REDUCTION = 0.4f;
    float BLOCK_SPEED_MULT = 0.4f;
    float BAR_X = 17.0f;
    float BAR_Y = 75.0f;
    float BAR_W = 200.0f;
    float BAR_H = 16.0f;
    float BAR_PADDING = 2.0f;
    Color COL_BG = Color(0, 0, 60);
    Color COL_FILL = Color(80, 160, 255);
    Color COL_BLAST_FILL = Color(180, 80, 255);
    Color COL_FRAME = Color(200, 220, 255);
    Color COL_TEXT = Color(255, 255, 255);
    Color COL_EMPTY = Color(40, 40, 80);
    Color COL_BLOCK = Color(255, 200, 50);
};








