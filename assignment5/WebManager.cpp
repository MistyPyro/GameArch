#include "WebManager.h"
#include "WebProjectile.h"
#include "WebEvent.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "BossManager.h"
#include "PlayerManager.h"
#include "EventSystem.h"
#include "GameEvent.h"
#include "MenuEvent.h"
#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include "Font.h"
#include "ScoreEvent.h"
#include "ConsumingEvent.h"
#include "LanguageManager.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include "FileManager.h"
#include "PhysicsManager.h"
#include "CollisionEvent.h"
#include "Collider.h"

WebManager* WebManager::mpsInstance = nullptr;

WebManager* WebManager::createInstance(EnemyManager* pEnemyManager)
{
    if (mpsInstance == nullptr)
        mpsInstance = new WebManager(pEnemyManager);
    return mpsInstance;
}

void  WebManager::destroyInstance()
{
    if (mpsInstance)
    {
        mpsInstance->destroy();
        delete mpsInstance;
        mpsInstance = nullptr;
    }
}


WebManager::WebManager(EnemyManager* pEnemyManager) : mpEnemyManager(pEnemyManager)
{
    EventSystem::getInstance()->addListener((EventType)WEB_FIRE_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)WEB_BLAST_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)BLOCK_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_DIFFICULTY_EVENT, this);
}

void WebManager::init(Font* pFont)
{
    FileManager::getInstance()->readAndImportFile(DATA_FILE);
    mpFont = pFont;
}

void WebManager::cleanup()
{
    for (int i = 0; i < mPool.size(); i++)
    {
        WebProjectile* p = mPool.getAll()[i];
        if (p->isActive()) p->mActive = false;
    }
    mBlastActive = false;
    mBlastTimer = 0.0f;
    mBlocking = false;
    mWebMeter = mWebMeterMax;
    mMeterWasEmpty = false;
    mRegenDelayTimer = 0.0f;
}

void WebManager::destroy()
{
    EventSystem::getInstance()->removeListenerFromAllEvents(this);
    mpFont = nullptr;
    mpBossManager = nullptr;
    mpPlayerManager = nullptr;
    mpEnemyManager = nullptr;
}

void WebManager::setDifficulty(int difficulty)
{
    switch (difficulty)
    {
    case 0:  mRegenRate = 12.0f; break;
    case 2:  mRegenRate = 5.0f;  break;
    default: mRegenRate = 8.0f;  break;
    }
    std::cout << "WebManager: regen=" << mRegenRate << " (diff " << difficulty << ")\n";
}

Vector2D WebManager::getPlayerOrigin() const
{
    if (mpPlayerManager) return mpPlayerManager->getPosition();
    return Vector2D(400, 300);
}

void WebManager::tryConsumeWeb(float cost)
{
    mWebMeter -= cost;
    mRegenDelayTimer = REGEN_DELAY_MS;
    if (mWebMeter <= 0.0f)
    {
        mWebMeter = 0.0f;
        if (!mMeterWasEmpty)
        {
            mMeterWasEmpty = true;
            EventSystem::getInstance()->fireEvent(WebMeterEmptyEvent());
        }
    }
}

void WebManager::fireWeb(Vector2D /*origin*/, Vector2D target)
{
    if (mWebMeter < WEB_COST) { std::cout << "WebManager: not enough web!\n"; return; }


    Vector2D playerOrigin = getPlayerOrigin();
    tryConsumeWeb(WEB_COST);

    float dx = target.getX() - playerOrigin.getX();
    float dy = target.getY() - playerOrigin.getY();
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 1.0f) return;

    Vector2D dir(dx / dist, dy / dist);

    // Find which slot the pool will use before acquiring
    WebProjectile* const* all = mPool.getAll();
    int slotIndex = -1;
    for (int i = 0; i < mPool.size(); i++)
        if (!all[i]->isActive()) { slotIndex = i; break; }

    WebProjectile* pWeb = mPool.acquire(playerOrigin, dir, WEB_SPEED);
    if (!pWeb) { std::cout << "WebManager: pool full, shot dropped\n"; return; }

    // Use slot index as unique collider key
    std::string colliderKey = "web" + std::to_string(slotIndex);
    PhysicsManager::getInstance()->createCollider(colliderKey, HIT_RADIUS, HIT_RADIUS, pWeb->getPosition(), true, pWeb);

    EventSystem::getInstance()->fireEvent(ConsumingEvent(0.0f, "spawn"));

}

void WebManager::checkWebCollisions()
{
    if (!mpEnemyManager) return;
    auto& enemies = mpEnemyManager->getEnemies();

    WebProjectile* const* all = mPool.getAll();
    int poolSize = mPool.size();

    // Boss
    if (mpBossManager && mpBossManager->isBossAlive())
    {
        for (int i = 0; i < poolSize; i++)
        {
            WebProjectile* pWeb = all[i];
            if (!pWeb->isActive()) continue;

            // Use boss CENTER not top-left corner
            // Boss sprite is ~257px wide/tall so center offset is ~128
            const float BOSS_SPRITE_HALF = 128.0f;
            Vector2D bossCenter(
                mpBossManager->getBossPosition().getX() + BOSS_SPRITE_HALF,
                mpBossManager->getBossPosition().getY() + BOSS_SPRITE_HALF);

            float dx = bossCenter.getX() - pWeb->getPosition().getX();
            float dy = bossCenter.getY() - pWeb->getPosition().getY();
            float dist = std::sqrt(dx * dx + dy * dy);

            // Use a larger hit radius for the boss 150px feels right for a 257px sprite
            if (dist <= 150.0f)
            {
                pWeb->mActive = false;
                mpBossManager->damageBoss(WEB_DAMAGE);
                EventSystem::getInstance()->fireEvent(ScoreEvent(5));
                EventSystem::getInstance()->fireEvent(ConsumingEvent(0.0f, "destroy"));
                std::cout << "Web hit boss!\n";
            }
        }
    }


    // Regular enemies
    for (int i = 0; i < poolSize; i++)
    {
        WebProjectile* pWeb = all[i];
        if (!pWeb->isActive()) continue;

        for (Enemy* pEnemy : enemies)
        {
            if (pEnemy->isDead()) continue;
            float centerX = pEnemy->getPosition().getX() + 66.0f;
            float centerY = pEnemy->getPosition().getY() + 66.0f;
            float dx = centerX - pWeb->getPosition().getX();
            float dy = centerY - pWeb->getPosition().getY();
           // float dx = pEnemy->getPosition().getX() - pWeb->getPosition().getX();
           // float dy = pEnemy->getPosition().getY() - pWeb->getPosition().getY();
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist <= HIT_RADIUS)
            {
                pWeb->mActive = false;
                pWeb->mHit = true;
                pEnemy->takeDamage(WEB_DAMAGE);
                EventSystem::getInstance()->fireEvent(WebHitEvent());
                EventSystem::getInstance()->fireEvent(ScoreEvent(5));
                EventSystem::getInstance()->fireEvent(ConsumingEvent(0.0f, "destroy"));
                std::cout << "Web hit enemy for " << WEB_DAMAGE << "!\n";
                break;
            }
        }
    }
}

void WebManager::fireBlast(Vector2D /*origin*/, Vector2D target)
{
    if (mWebMeter < BLAST_COST)
    {
        std::cout << "WebManager: not enough web for blast!\n";
        return;
    }

    Vector2D playerOrigin = getPlayerOrigin();
    tryConsumeWeb(BLAST_COST);

    float dx = target.getX() - playerOrigin.getX();
    float dy = target.getY() - playerOrigin.getY();
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 1.0f) return;

    mBlastOrigin = playerOrigin;
    mBlastDirection = Vector2D(dx / dist, dy / dist);
    mBlastActive = true;
    mBlastTimer = 0.0f;
    mBlastWidth = 4.0f;
    mBlastHitThisFrame = false;

    EventSystem::getInstance()->fireEvent(ConsumingEvent(0.0f, "consuming"));
    std::cout << "WebManager: BLAST fired!\n";
}

void WebManager::updateBlast(double dt)
{
    if (!mBlastActive) return;
    mBlastTimer += (float)dt;

    float progress = mBlastTimer / BLAST_DURATION_MS;
    if (progress < 0.3f)
        mBlastWidth = BLAST_MAX_WIDTH * (progress / 0.3f);
    else
        mBlastWidth = BLAST_MAX_WIDTH * (1.0f - (progress - 0.3f) / 0.7f);

    checkBlastCollisions();

    if (mBlastTimer >= BLAST_DURATION_MS)
    {
        mBlastActive = false;
        mBlastHitThisFrame = false;
    }
}

void WebManager::checkBlastCollisions()
{
    if (!mpEnemyManager || !mBlastActive) return;
    auto& enemies = mpEnemyManager->getEnemies();
    const float BEAM_LENGTH = 900.0f;

    for (Enemy* pEnemy : enemies)
    {
        if (pEnemy->isDead()) continue;

        Vector2D ep = pEnemy->getPosition();
        float ex = ep.getX() - mBlastOrigin.getX();
        float ey = ep.getY() - mBlastOrigin.getY();

        float proj = ex * mBlastDirection.getX() + ey * mBlastDirection.getY();
        if (proj < 0 || proj > BEAM_LENGTH) continue;

        float perpX = ex - mBlastDirection.getX() * proj;
        float perpY = ey - mBlastDirection.getY() * proj;
        float perp = std::sqrt(perpX * perpX + perpY * perpY);

        if (perp <= BLAST_HIT_RADIUS)
        {
            pEnemy->takeDamage(BLAST_DAMAGE);
            EventSystem::getInstance()->fireEvent(WebHitEvent());
            EventSystem::getInstance()->fireEvent(ScoreEvent(15));
            EventSystem::getInstance()->fireEvent(ConsumingEvent(0.0f, "consuming"));
        }
    }
}

void WebManager::drawBlast()
{
    if (!mBlastActive) return;

    SDL_Renderer* pR = GraphicsBuffer::getRenderer();
    if (!pR) return;
    SDL_SetRenderTarget(pR, nullptr);

    float progress = mBlastTimer / BLAST_DURATION_MS;
    Uint8 alpha = (Uint8)(255 * (1.0f - progress));
    float halfW = mBlastWidth / 2.0f;
    const float BEAM_LENGTH = 900.0f;

    float perpX = -mBlastDirection.getY();
    float perpY = mBlastDirection.getX();
    float ox = mBlastOrigin.getX();
    float oy = mBlastOrigin.getY();
    float ex = ox + mBlastDirection.getX() * BEAM_LENGTH;
    float ey = oy + mBlastDirection.getY() * BEAM_LENGTH;

    for (int i = -(int)halfW; i <= (int)halfW; i += 2)
    {
        float offX = perpX * i;
        float offY = perpY * i;
        SDL_SetRenderDrawColor(pR, 140, 60, 255, (Uint8)(alpha * 0.5f));
        SDL_RenderLine(pR, (int)(ox + offX), (int)(oy + offY), (int)(ex + offX), (int)(ey + offY));
    }
    int coreW = (int)(halfW * 0.4f);
    for (int i = -coreW; i <= coreW; i++)
    {
        float offX = perpX * i;
        float offY = perpY * i;
        SDL_SetRenderDrawColor(pR, 200, 220, 255, alpha);
        SDL_RenderLine(pR, (int)(ox + offX), (int)(oy + offY), (int)(ex + offX), (int)(ey + offY));
    }
}

void WebManager::update(double dt)
{
    if (mRegenDelayTimer > 0.0f)
        mRegenDelayTimer -= (float)dt;

    float regenMult = mBlocking ? 0.5f : 1.0f;
    if (mRegenDelayTimer <= 0.0f && mWebMeter < mWebMeterMax)
    {
        mWebMeter += mRegenRate * regenMult * ((float)dt / 1000.0f);
        if (mWebMeter >= mWebMeterMax)
        {
            mWebMeter = mWebMeterMax;
            if (mMeterWasEmpty)
            {
                mMeterWasEmpty = false;
                EventSystem::getInstance()->fireEvent(WebMeterRefilledEvent());
            }
        }
    }

    WebProjectile* const* all = mPool.getAll();
    for (int i = 0; i < mPool.size(); i++)
    {
        if (all[i]->isActive())
        {
            all[i]->update(dt);

            std::string key = "web" + std::to_string(i);
            Collider* pCol = PhysicsManager::getInstance()->getCollider(key);

            if (pCol) pCol->setCenter(all[i]->getPosition());

        }
        else
        {
            // Web just became inactive — remove its collider
            std::string key = "web" + std::to_string(i);
            if (PhysicsManager::getInstance()->getCollider(key))
                PhysicsManager::getInstance()->deleteCollider(key);
        }
    }

    checkWebCollisions();

    updateBlast(dt);
}

void WebManager::draw(Vector2D cameraOffset)
{
    WebProjectile* const* all = mPool.getAll();
    for (int i = 0; i < mPool.size(); i++)
        if (all[i]->isActive()) all[i]->draw(cameraOffset);
    drawBlast();
}

void WebManager::drawHUD()
{
    drawMeter();
}

void WebManager::drawMeter()
{
    SDL_Renderer* pR = GraphicsBuffer::getRenderer();
    if (!pR) return;
    SDL_SetRenderTarget(pR, nullptr);

    float fraction = mWebMeter / mWebMeterMax;
    if (fraction < 0.0f) fraction = 0.0f;
    if (fraction > 1.0f) fraction = 1.0f;

    SDL_SetRenderDrawColor(pR, COL_BG.getR(), COL_BG.getG(), COL_BG.getB(), 255);
    SDL_FRect bg{ BAR_X, BAR_Y, BAR_W, BAR_H };
    SDL_RenderFillRect(pR, &bg);

    Color fillCol = COL_FILL;
    if (mBlocking) fillCol = COL_BLOCK;
    else if (mWebMeter >= BLAST_COST) fillCol = COL_BLAST_FILL;
    if (mMeterWasEmpty) fillCol = COL_EMPTY;

    SDL_SetRenderDrawColor(pR, fillCol.getR(), fillCol.getG(), fillCol.getB(), 255);
    float fillW = (BAR_W - BAR_PADDING * 2) * fraction;
    SDL_FRect fill{ BAR_X + BAR_PADDING, BAR_Y + BAR_PADDING, fillW, BAR_H - BAR_PADDING * 2 };
    SDL_RenderFillRect(pR, &fill);

    float markerX = BAR_X + BAR_PADDING + (BAR_W - BAR_PADDING * 2) * (BLAST_COST / mWebMeterMax);
    SDL_SetRenderDrawColor(pR, 255, 255, 100, 200);
    SDL_RenderLine(pR, (int)markerX, (int)BAR_Y, (int)markerX, (int)(BAR_Y + BAR_H));

    SDL_SetRenderDrawColor(pR, COL_FRAME.getR(), COL_FRAME.getG(), COL_FRAME.getB(), 255);
    SDL_RenderRect(pR, &bg);

    if (mpFont)
    {
        std::string label;
        if (mBlastActive) label = LS(StringID::HUD_BLAST);
        else if (mBlocking) label = LS(StringID::HUD_BLOCK);
        else label = LS(StringID::HUD_WEB);

        GraphicsSystem::getBackBuffer()->writeText(Vector2D(BAR_X + BAR_W + 8, BAR_Y), *mpFont, mBlocking ? COL_BLOCK : COL_TEXT, label);
    }
}

void WebManager::handleEvent(const Event& theEvent)
{
    switch (theEvent.getType())
    {
    case WEB_FIRE_EVENT:
    {
        const WebFireEvent& e = static_cast<const WebFireEvent&>(theEvent);
        fireWeb(e.getOrigin(), e.getTarget());
        break;
    }
    case WEB_BLAST_EVENT:
    {
        const WebBlastEvent& e = static_cast<const WebBlastEvent&>(theEvent);
        fireBlast(e.getOrigin(), e.getTarget());
        break;
    }
    case BLOCK_EVENT:
    {
        const BlockEvent& e = static_cast<const BlockEvent&>(theEvent);
        mBlocking = e.isBlocking();
        break;
    }
    case MENU_DIFFICULTY_EVENT:
    {
        const MenuDifficultyEvent& e =
            static_cast<const MenuDifficultyEvent&>(theEvent);
        setDifficulty(e.getDifficulty());
        break;
    }
   /*case COLLISION_EVENT:
        const CollisionEvent& col = static_cast<const CollisionEvent&>(theEvent);
        if (col.getCollider1Tag() != "web") //needs to detect if event involves web and enemy, then damage enemy
            return;*/
    default:
        break;
    }
}


