#include "WinScreen.h"
#include "GraphicsBuffer.h"
#include "GraphicsSystem.h"
#include "Font.h"
#include "LanguageManager.h"
#include <SDL3/SDL.h>
#include <iostream>
#include "FileManager.h"

WinScreen* WinScreen::mpsInstance = nullptr;

WinScreen* WinScreen::createInstance()
{
    if (mpsInstance == nullptr)
        mpsInstance = new WinScreen;
    return mpsInstance;
}

void WinScreen::destroyInstance()
{
    delete mpsInstance;
    mpsInstance = nullptr;
}

void WinScreen::init(Font* pFont, unsigned int screenW, unsigned int screenH)
{
    FileManager::getInstance()->readAndImportFile(DATA_FILE);
    mpFont = pFont;
    mScreenW = screenW;
    mScreenH = screenH;
}

void WinScreen::cleanup()
{
    mpFont = nullptr;
}

void WinScreen::activate(WinType type, int levelJustCompleted, int score)
{
    mActive = true;
    mWinType = type;
    mLevel = levelJustCompleted;
    mScore = score;
    mFadeTimer = 0.0f;
    mWantsNextLevel = false;
    mWantsMainMenu = false;
    mMouseWasDown = false;
    std::cout << "WinScreen: activated (type=" << (int)type << " level=" << levelJustCompleted << " score=" << score << ")\n";
}

void WinScreen::reset()
{
    mActive = false;
    mWantsNextLevel = false;
    mWantsMainMenu = false;
    mFadeTimer = 0.0f;
}

bool WinScreen::drawButton(const std::string& label, float x, float y, float w, float h)
{
    SDL_Renderer* pR = GraphicsBuffer::getRenderer();
    if (!pR) return false;

    bool hovered = (mMouseX >= x && mMouseX <= x + w && mMouseY >= y && mMouseY <= y + h);

    Color col = hovered ? COL_BTN_HOV : COL_BTN;

    SDL_SetRenderTarget(pR, nullptr);
    SDL_SetRenderDrawColor(pR, col.getR(), col.getG(), col.getB(), 255);
    SDL_FRect rect{ x, y, w, h };
    SDL_RenderFillRect(pR, &rect);

    SDL_SetRenderDrawColor(pR, 80, 200, 80, 150);
    SDL_RenderRect(pR, &rect);

    if (mpFont)
        GraphicsSystem::getBackBuffer()->writeText(Vector2D(x + w / 2.0f, y + h / 2.0f - 10), *mpFont, COL_TEXT, label, Font::CENTER);

    return hovered && mMousePressed && !mMouseWasDown;
}

void WinScreen::update(double dtMs)
{
    if (!mActive) return;
    mFadeTimer += (float)dtMs;

    SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mMouseX, &mMouseY);
    bool isDown = (buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0;
    mMousePressed = isDown;
    if (!isDown) mMouseWasDown = false;
}

void WinScreen::draw()
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

    if (progress < 0.4f)
    {
        SDL_SetRenderDrawBlendMode(pR, SDL_BLENDMODE_NONE);
        return;
    }

    float pw = 420, ph = 320;
    float px = (mScreenW - pw) / 2.0f;
    float py = (mScreenH - ph) / 2.0f;

    SDL_SetRenderDrawColor(pR, COL_PANEL.getR(), COL_PANEL.getG(), COL_PANEL.getB(), panelAlpha);
    SDL_FRect panel{ px, py, pw, ph };
    SDL_RenderFillRect(pR, &panel);

    SDL_SetRenderDrawBlendMode(pR, SDL_BLENDMODE_NONE);

    if (!mpFont) return;

    bool isFinal = (mWinType == WinType::FINAL_WIN);
    std::string titleStr = isFinal ? LS(StringID::WIN_FINAL_TITLE) : LS(StringID::WIN_TITLE);

    GraphicsSystem::getBackBuffer()->writeText(Vector2D(mScreenW / 2.0f, py + 30), *mpFont, isFinal ? COL_GOLD : COL_TITLE, titleStr, Font::CENTER);

    std::string scoreStr = LS(StringID::WIN_SCORE) + ": " + std::to_string(mScore);
    GraphicsSystem::getBackBuffer()->writeText(Vector2D(mScreenW / 2.0f, py + 80), *mpFont, COL_TEXT, scoreStr, Font::CENTER);

    if (!isFinal)
    {
        std::string levelStr = "Level " + std::to_string(mLevel + 1) + " Complete";
        GraphicsSystem::getBackBuffer()->writeText(Vector2D(mScreenW / 2.0f, py + 115), *mpFont, COL_TEXT, levelStr, Font::CENTER);
    }

    if (progress >= 0.8f)
    {
        float bw = 260, bh = 48;
        float bx = px + (pw - bw) / 2.0f;

        if (!isFinal)
        {
            std::string nextStr = LS(StringID::WIN_NEXT_LEVEL);
            if (drawButton(nextStr, bx, py + 170, bw, bh))
            {
                mWantsNextLevel = true;
                std::cout << "WinScreen: next level chosen\n";
            }

            std::string menuStr = LS(StringID::WIN_MAIN_MENU);
            if (drawButton(menuStr, bx, py + 240, bw, bh))
            {
                mWantsMainMenu = true;
                std::cout << "WinScreen: main menu chosen\n";
            }
        }
        else
        {
            std::string menuStr = LS(StringID::WIN_MAIN_MENU);
            if (drawButton(menuStr, bx, py + 210, bw, bh))
            {
                mWantsMainMenu = true;
                std::cout << "WinScreen: main menu chosen (final win)\n";
            }
        }
    }

    if (mMousePressed) mMouseWasDown = true;
}
