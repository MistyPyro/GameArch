#include "BossManager.h"
#include "Boss.h"
#include "Camera.h"
#include "PlayerManager.h"
#include "Unit.h"
#include "EventSystem.h"
#include "BossEvent.h"
#include "ConsumingEvent.h"
#include "GraphicsBuffer.h"
#include "GraphicsSystem.h"
#include "Font.h"
#include "Sprite.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include "GraphicsBufferManager.h"
#include "FileManager.h"

BossManager* BossManager::mpsInstance = nullptr;

BossManager* BossManager::createInstance(PlayerManager* pPlayerManager, Camera* pCamera)
{
    if (mpsInstance == nullptr)
        mpsInstance = new BossManager(pPlayerManager, pCamera);
    return mpsInstance;
}

void BossManager::destroyInstance()
{
    delete mpsInstance;
    mpsInstance = nullptr;
}

BossManager::BossManager(PlayerManager* pPlayerManager, Camera* pCamera) : mpPlayerManager(pPlayerManager), mpCamera(pCamera)
{
    EventSystem::getInstance()->addListener((EventType)BOSS_DEAD_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)BOSS_PHASE_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)BOSS_ATTACK_EVENT, this);
}

void BossManager::init(Font* pFont)
{
    FileManager::getInstance()->readAndImportFile(DATA_FILE);
    mpFont = pFont;
}

void BossManager::cleanup()
{
    EventSystem::getInstance()->removeListenerFromAllEvents(this);
    delete mpBoss;
    mpBoss = nullptr;
    mpFont = nullptr;
}

std::vector<Animation> BossManager::buildAnims(GraphicsBuffer* pBuf, unsigned int frameW, unsigned int frameH, unsigned int cols, unsigned int rows, bool loops)
{
    std::vector<Sprite> sprites;
    for (unsigned int row = 0; row < rows; row++)
        for (unsigned int col = 0; col < cols; col++)
            sprites.push_back(Sprite(pBuf,
                Vector2D((float)(col * frameW), (float)(row * frameH)),
                frameW, frameH));

    Animation anim(loops);
    for (auto& s : sprites) anim.addSprite(s);

    return { anim };
}

void BossManager::loadBossAnimations(
    GraphicsBuffer* pIdleBuf, unsigned int idleW, unsigned int idleH, unsigned int idleCols, unsigned int idleRows,
    GraphicsBuffer* pWalkBuf, unsigned int walkW, unsigned int walkH, unsigned int walkCols, unsigned int walkRows,
    GraphicsBuffer* pHurtBuf, unsigned int hurtW, unsigned int hurtH, unsigned int hurtCols, unsigned int hurtRows,
    GraphicsBuffer* pDyingBuf, unsigned int dyingW, unsigned int dyingH, unsigned int dyingCols, unsigned int dyingRows)
{
    mIdleAnims = buildAnims(pIdleBuf, idleW, idleH, idleCols, idleRows, true);
    mWalkAnims = buildAnims(pWalkBuf, walkW, walkH, walkCols, walkRows, true);
    mHurtAnims = buildAnims(pHurtBuf, hurtW, hurtH, hurtCols, hurtRows, false);
    mDyingAnims = buildAnims(pDyingBuf, dyingW, dyingH, dyingCols, dyingRows, false);
}

void BossManager::addAttackAnimation(GraphicsBuffer* pBuf, unsigned int frameW, unsigned int frameH, unsigned int cols, unsigned int rows, bool loops)
{
    auto anims = buildAnims(pBuf, frameW, frameH, cols, rows, loops);
    for (auto& a : anims)
        mAttackAnims.push_back(a);
}

void BossManager::spawnBoss(Vector2D pos, int maxHp)
{
    delete mpBoss;
    mpBoss = new Boss(mIdleAnims, mWalkAnims, mAttackAnims, mHurtAnims, mDyingAnims, pos, maxHp);
    std::cout << "BossManager: boss spawned at " << pos.getX() << "," << pos.getY() << "\n";
    mBossWasSpawned = true;
}

void BossManager::update(double dt)
{
    if (!mpBoss || !mpPlayerManager || !mpPlayerManager->getPlayer()) return;

    Vector2D playerPos = mpPlayerManager->getPlayer()->getPosition();
    mpBoss->update(dt, playerPos);
    if (mpBoss->isHitboxActive())
    {
        Vector2D bossCenter = getBossCenter();
        float dx = playerPos.getX() - bossCenter.getX();
        float dy = playerPos.getY() - bossCenter.getY();
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist <= mpBoss->getAttackRange())
        {
            mpPlayerManager->takeDamage(mpBoss->getAttackDamage());
            mpBoss->mHitDealtThisSwing = true;
            EventSystem::getInstance()->fireEvent(ConsumingEvent(0.0f, "destroy"));
            std::cout << "boss hit player for " << mpBoss->getAttackDamage() << "!\n";
        }
    }

    if (mpBoss->isDead())
    {
        delete mpBoss;
        mpBoss = nullptr;
    }
}

void BossManager::draw()
{
    if (!mpBoss || !mpCamera) return;

    mpBoss->draw(mpCamera->getOffset());
}

void BossManager::drawBossHealthBar()
{
    if (!mpBoss) return;

    SDL_Renderer* pR = GraphicsBuffer::getRenderer();
    if (!pR) return;

    SDL_SetRenderTarget(pR, nullptr);
    float fraction = (float)mpBoss->getHp() / (float)mpBoss->getMaxHp();
    if (fraction < 0) fraction = 0;

    if (fraction > 1) fraction = 1;

    Uint8 r = 50, g = 200, b = 50;
    if (mpBoss->getPhase() == Boss::BossPhase::TWO)
    {
        r = 255; g = 140; b = 0;
    }
    else if (mpBoss->getPhase() == Boss::BossPhase::THREE)
    {
        r = 220; g = 20;  b = 20;
    }
    SDL_SetRenderDrawColor(pR, 30, 0, 0, 255);
    SDL_FRect bg{ BAR_X, BAR_Y, BAR_W, BAR_H };
    SDL_RenderFillRect(pR, &bg);
    SDL_SetRenderDrawColor(pR, r, g, b, 255);
    float fillW = (BAR_W - BAR_PAD * 2) * fraction;
    SDL_FRect fill{ BAR_X + BAR_PAD, BAR_Y + BAR_PAD, fillW, BAR_H - BAR_PAD * 2 };
    SDL_RenderFillRect(pR, &fill);
    SDL_SetRenderDrawColor(pR, 255, 255, 255, 150);
    SDL_RenderRect(pR, &bg);
    if (mpFont)
        GraphicsSystem::getBackBuffer()->writeText(Vector2D(BAR_X + BAR_W / 2.0f, BAR_Y - 22), *mpFont, Color(255, 50, 50), "VENOM", Font::CENTER);
}

void BossManager::drawHUD()
{
    drawBossHealthBar();
}

void BossManager::handleEvent(const Event& theEvent)
{
    switch (theEvent.getType())
    {
    case BOSS_DEAD_EVENT:
        std::cout << "BossManager: boss is dead!\n";
        EventSystem::getInstance()->fireEvent(ConsumingEvent(0.0f, "consuming"));
        break;
    case BOSS_PHASE_EVENT:
    {
        const BossPhaseEvent& e = static_cast<const BossPhaseEvent&>(theEvent);
        std::cout << "BossManager: entering phase " << e.getPhase() << "\n";
        break;
    }
    case BOSS_ATTACK_EVENT:
    {
        const BossAttackEvent& e = static_cast<const BossAttackEvent&>(theEvent);
        std::cout << "Boss attack type: " << e.getAttackType() << "\n";
        break;
    }
    default:
        break;
    }
}

bool BossManager::isBossAlive() const
{
    return mpBoss != nullptr && !mpBoss->isDead();
}

Vector2D BossManager::getBossPosition() const
{
    return mpBoss ? mpBoss->getPosition() : Vector2D(0, 0);
}

void BossManager::damageBoss(int amount)
{
    if (mpBoss)
        mpBoss->takeDamage(amount);
}


std::vector<Animation> BossManager::buildAnimsFromFiles(const std::vector<std::string>& filePaths, unsigned int frameW, unsigned int frameH, bool loops)
{
    Animation anim(loops);
    for (const std::string& path : filePaths)
    {

        std::string key = path;
        size_t slash = key.find_last_of("/\\");
        if (slash != std::string::npos)
            key = key.substr(slash + 1);

        GraphicsBuffer* pBuf = mpGraphicsBufferManager->getGraphicsBuffer(key);
        if (!pBuf)
        {
            mpGraphicsBufferManager->createAndManageGraphicsBuffer(key, path);
            pBuf = mpGraphicsBufferManager->getGraphicsBuffer(key);
        }
        if (!pBuf)
        {
            std::cout << "BossManager: failed to load " << path << "\n";
            continue;
        }

        Sprite sprite(pBuf, Vector2D(0, 0), frameW, frameH);
        anim.addSprite(sprite);
    }
    return { anim };
}

void BossManager::loadBossAnimationsFromFiles(
    const std::vector<std::string>& idleFiles, unsigned int idleW, unsigned int idleH,
    const std::vector<std::string>& walkFiles, unsigned int walkW, unsigned int walkH,
    const std::vector<std::string>& hurtFiles, unsigned int hurtW, unsigned int hurtH,
    const std::vector<std::string>& dyingFiles, unsigned int dyingW, unsigned int dyingH)
{
    mIdleAnims = buildAnimsFromFiles(idleFiles, idleW, idleH, true);
    mWalkAnims = buildAnimsFromFiles(walkFiles, walkW, walkH, true);
    mHurtAnims = buildAnimsFromFiles(hurtFiles, hurtW, hurtH, false);
    mDyingAnims = buildAnimsFromFiles(dyingFiles, dyingW, dyingH, false);
}

void BossManager::addAttackAnimationFromFiles(
    const std::vector<std::string>& files,
    unsigned int frameW, unsigned int frameH,
    bool loops)
{
    auto anims = buildAnimsFromFiles(files, frameW, frameH, loops);
    for (auto& a : anims)
        mAttackAnims.push_back(a);
}