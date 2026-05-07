#include "DeathScreen.h"
#include "GraphicsBuffer.h"
#include "GraphicsSystem.h"
#include "Font.h"
#include "LanguageManager.h"
#include <SDL3/SDL.h>
#include <iostream>
#include "FileManager.h"

DeathScreen* DeathScreen::mpsInstance = nullptr;

DeathScreen* DeathScreen::createInstance()
{
    if (mpsInstance == nullptr)
        mpsInstance = new DeathScreen();
    return mpsInstance;
}

void DeathScreen::destroyInstance()
{
    delete mpsInstance;
    mpsInstance = nullptr;
}

void DeathScreen::init(Font* pFont, unsigned int screenW, unsigned int screenH)
{
    FileManager::getInstance()->readAndImportFile(DATA_FILE);
    mpFont = pFont;
    mScreenW = screenW;
    mScreenH = screenH;
}

void DeathScreen::cleanup()
{
    mpFont = nullptr;
}

void DeathScreen::activate()
{
    mActive = true;
    mFadeTimer = 0.0f;
    mWantsRestart = false;
    mWantsQuit = false;
    mMouseWasDown = false;
    std::cout << "DeathScreen: activated\n";
}

void DeathScreen::reset()
{
    mActive = false;
    mFadeTimer = 0.0f;
    mWantsRestart = false;
    mWantsQuit = false;
}

bool DeathScreen::drawButton(const std::string& label, float x, float y, float w, float h)
{
    SDL_Renderer* pR = GraphicsBuffer::getRenderer();
    if (!pR) return false;

    bool hovered = (mMouseX >= x && mMouseX <= x + w && mMouseY >= y && mMouseY <= y + h);

    Color col = hovered ? COL_BTN_HOV : COL_BTN;

    SDL_SetRenderTarget(pR, nullptr);
    SDL_SetRenderDrawColor(pR, col.getR(), col.getG(), col.getB(), 255);
    SDL_FRect rect{ x, y, w, h };
    SDL_RenderFillRect(pR, &rect);

    SDL_SetRenderDrawColor(pR, 200, 50, 50, 150);
    SDL_RenderRect(pR, &rect);

    if (mpFont)
        GraphicsSystem::getBackBuffer()->writeText(Vector2D(x + w / 2.0f, y + h / 2.0f - 10), *mpFont, COL_TEXT, label, Font::CENTER);

    return hovered && mMousePressed && !mMouseWasDown;
}

void DeathScreen::update(double dtMs)
{
    if (!mActive) return;

    mFadeTimer += (float)dtMs;

    SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mMouseX, &mMouseY);
    bool isDown = (buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0;
    mMousePressed = isDown;
    if (!isDown) mMouseWasDown = false;
}

void DeathScreen::draw()
{
    if (!mActive) return;

    SDL_Renderer* pR = GraphicsBuffer::getRenderer();
    if (!pR) return;

    float progress = mFadeTimer / FADE_DURATION;
    if (progress > 1.0f) progress = 1.0f;
    Uint8 overlayAlpha = (Uint8)(COL_OVERLAY.getA() * progress);
    Uint8 panelAlpha = (Uint8)(COL_PANEL.getA() * progress);

    SDL_SetRenderTarget(pR, nullptr);
    SDL_SetRenderDrawBlendMode(pR, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(pR, COL_OVERLAY.getR(), COL_OVERLAY.getG(), COL_OVERLAY.getB(), overlayAlpha);
    SDL_FRect full{ 0, 0, (float)mScreenW, (float)mScreenH };
    SDL_RenderFillRect(pR, &full);

    if (progress < 0.5f)
    {
        SDL_SetRenderDrawBlendMode(pR, SDL_BLENDMODE_NONE);
        return;
    }

    float pw = 380, ph = 300;
    float px = (mScreenW - pw) / 2.0f;
    float py = (mScreenH - ph) / 2.0f;
    SDL_SetRenderDrawColor(pR, COL_PANEL.getR(), COL_PANEL.getG(), COL_PANEL.getB(), panelAlpha);
    SDL_FRect panel{ px, py, pw, ph };
    SDL_RenderFillRect(pR, &panel);

    SDL_SetRenderDrawBlendMode(pR, SDL_BLENDMODE_NONE);

    if (!mpFont) return;

    std::string titleStr = LS(StringID::DEATH_TITLE);
    std::string restartStr = LS(StringID::DEATH_RESTART);
    std::string quitStr = LS(StringID::DEATH_QUIT_MAIN);

    GraphicsSystem::getBackBuffer()->writeText(Vector2D(mScreenW / 2.0f, py + 30), *mpFont, COL_TITLE, titleStr, Font::CENTER);

    if (progress >= 0.8f)
    {
        float bw = 240, bh = 48;
        float bx = px + (pw - bw) / 2.0f;

        if (drawButton(restartStr, bx, py + 140, bw, bh))
        {
            mWantsRestart = true;
            std::cout << "DeathScreen: restart chosen\n";
        }

        if (drawButton(quitStr, bx, py + 210, bw, bh))
        {
            mWantsQuit = true;
            std::cout << "DeathScreen: quit to main chosen\n";
        }
    }

    if (mMousePressed) mMouseWasDown = true;
}
