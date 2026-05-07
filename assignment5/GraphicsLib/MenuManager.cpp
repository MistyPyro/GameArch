#include "MenuManager.h"
#include "../MenuEvent.h"
#include "../GameEvent.h"
#include "EventSystem.h"
#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include "Font.h"
#include "../FileManager.h"
#include "../LanguageManager.h"
#include <SDL3/SDL.h>
#include <iostream>


MenuManager::MenuManager()
{
    EventSystem::getInstance()->addListener((EventType)MENU_PLAY_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_LOAD_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_OPTIONS_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_QUIT_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_RESUME_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_QUIT_TO_MAIN_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_PAUSE_EVENT, this);
}

void MenuManager::init(Font* pFont, unsigned int screenW, unsigned int screenH)
{
    mpFont = pFont;
    mScreenW = screenW;
    mScreenH = screenH;
}

void MenuManager::cleanup()
{
    EventSystem::getInstance()->removeListenerFromAllEvents(this);
    mpFont = nullptr;
    mpFileManager = nullptr;
}

void MenuManager::drawBackground()
{
    SDL_Renderer* pR = GraphicsBuffer::getRenderer();
    if (!pR) return;

    SDL_SetRenderTarget(pR, nullptr);
    SDL_SetRenderDrawBlendMode(pR, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(pR, COL_OVERLAY.getR(), COL_OVERLAY.getG(), COL_OVERLAY.getB(), COL_OVERLAY.getA());
    SDL_FRect full{ 0, 0, (float)mScreenW, (float)mScreenH };
    SDL_RenderFillRect(pR, &full);

    float pw = 500, ph = 520;
    float px = (mScreenW - pw) / 2.0f;
    float py = (mScreenH - ph) / 2.0f;
    SDL_SetRenderDrawColor(pR, COL_PANEL.getR(), COL_PANEL.getG(), COL_PANEL.getB(), COL_PANEL.getA());
    SDL_FRect panel{ px, py, pw, ph };
    SDL_RenderFillRect(pR, &panel);

    SDL_SetRenderDrawBlendMode(pR, SDL_BLENDMODE_NONE);
}

void MenuManager::drawTitle(const std::string& title)
{
    if (!mpFont) return;
    float x = mScreenW / 2.0f;
    float y = (mScreenH - 420) / 2.0f + 20.0f;
    GraphicsSystem::getBackBuffer()->writeText(Vector2D(x, y), *mpFont, COL_TITLE, title, Font::CENTER);
}

bool MenuManager::drawButton(const std::string& label,
    float x, float y, float w, float h,
    bool selected)
{
    SDL_Renderer* pR = GraphicsBuffer::getRenderer();
    if (!pR) return false;

    bool hovered = (mMouseX >= x && mMouseX <= x + w &&
        mMouseY >= y && mMouseY <= y + h);

    Color col = selected ? COL_BTN_SEL : (hovered ? COL_BTN_HOVER : COL_BTN);

    SDL_SetRenderTarget(pR, nullptr);
    SDL_SetRenderDrawColor(pR, col.getR(), col.getG(), col.getB(), 255);
    SDL_FRect rect{ x, y, w, h };
    SDL_RenderFillRect(pR, &rect);

    SDL_SetRenderDrawColor(pR, 255, 255, 255, 80);
    SDL_RenderRect(pR, &rect);

    if (mpFont)
        GraphicsSystem::getBackBuffer()->writeText(Vector2D(x + w / 2.0f, y + h / 2.0f - 10), *mpFont, COL_TEXT, label, Font::CENTER);

    return hovered && mMousePressed && !mMouseWasDown;
}

void MenuManager::drawToggle(const std::string& label, bool& value, float x, float y, float w, float h)
{
    if (mpFont)
        GraphicsSystem::getBackBuffer()->writeText(Vector2D(x, y + 4), *mpFont, COL_TEXT, label);

    float halfW = (w - 8) / 2.0f;
    float bx = x + w / 2.0f;

    std::string onStr = LS(StringID::OPT_ON);
    std::string offStr = LS(StringID::OPT_OFF);

    if (drawButton(onStr, bx, y, halfW, h, value)) value = true;
    if (drawButton(offStr, bx + halfW + 8, y, halfW, h, !value)) value = false;
}

void MenuManager::drawSlider(const std::string& label, int& value, float x, float y, float w, float h)
{
    SDL_Renderer* pR = GraphicsBuffer::getRenderer();
    if (!pR) return;

    if (mpFont)
        GraphicsSystem::getBackBuffer()->writeText(Vector2D(x, y + 4), *mpFont, COL_TEXT, label + ": " + std::to_string(value));

    float sliderX = x + w * 0.45f;
    float sliderW = w * 0.5f;

    SDL_SetRenderTarget(pR, nullptr);

    SDL_SetRenderDrawColor(pR, COL_SLIDER_BG.getR(), COL_SLIDER_BG.getG(), COL_SLIDER_BG.getB(), 255);
    SDL_FRect track{ sliderX, y + h / 2.0f - 3, sliderW, 6 };
    SDL_RenderFillRect(pR, &track);

    float fillW = sliderW * (value / 100.0f);
    SDL_SetRenderDrawColor(pR, COL_SLIDER_FILL.getR(), COL_SLIDER_FILL.getG(), COL_SLIDER_FILL.getB(), 255);
    SDL_FRect fill{ sliderX, y + h / 2.0f - 3, fillW, 6 };
    SDL_RenderFillRect(pR, &fill);

    if (mMousePressed &&
        mMouseY >= y && mMouseY <= y + h &&
        mMouseX >= sliderX && mMouseX <= sliderX + sliderW)
    {
        float t = (mMouseX - sliderX) / sliderW;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        value = (int)(t * 100);
    }
}

void MenuManager::drawSelector(const std::string& label,
    const std::vector<std::string>& options,
    int& selected,
    float x, float y, float w, float h)
{
    if (mpFont)
        GraphicsSystem::getBackBuffer()->writeText(
            Vector2D(x, y + 4), *mpFont, COL_TEXT, label);

    float optW = (w * 0.85f) / (float)options.size() - 4;
    float startX = x + w * 0.31f;

    for (int i = 0; i < (int)options.size(); i++)
    {
        float bx = startX + i * (optW + 4);
        if (drawButton(options[i], bx, y, optW, h, selected == i))
            selected = i;
    }
}

void MenuManager::fireOptionsEvents()
{
    LanguageManager::getInstance()->setLanguage((LanguageManager::Language)mLanguage);

    EventSystem::getInstance()->fireEvent(MenuDifficultyEvent(mDifficulty));
    EventSystem::getInstance()->fireEvent(MenuSoundEvent(mSoundOn, mSoundVolume));
    EventSystem::getInstance()->fireEvent(MenuMusicEvent(mMusicOn, mMusicVolume));
    EventSystem::getInstance()->fireEvent(MenuLanguageEvent(mLanguage));
}

void MenuManager::drawMainMenu()
{
    drawBackground();
    drawTitle("SPIDER-MAN");  

    float pw = 400;
    float px = (mScreenW - pw) / 2.0f;
    float py = (mScreenH - 420) / 2.0f;
    float bw = 280, bh = 44;
    float bx = px + (pw - bw) / 2.0f;
    float startY = py + 90;
    float gap = 56;

    std::string play = LS(StringID::MENU_PLAY);
    std::string load = LS(StringID::MENU_LOAD);
    std::string options = LS(StringID::MENU_OPTIONS);
    std::string quit = LS(StringID::MENU_QUIT);

    if (drawButton(play, bx, startY, bw, bh, false))
        EventSystem::getInstance()->fireEvent(MenuPlayEvent());

    if (drawButton(load, bx, startY + gap, bw, bh, false))
        EventSystem::getInstance()->fireEvent(MenuLoadEvent());

    if (drawButton(options, bx, startY + gap * 2, bw, bh, false))
        EventSystem::getInstance()->fireEvent(MenuOptionsEvent());

    if (drawButton(quit, bx, startY + gap * 3, bw, bh, false))
        EventSystem::getInstance()->fireEvent(MenuQuitEvent());
}

void MenuManager::drawOptionsMenu()
{
    drawBackground();
    std::string title = LS(StringID::OPT_TITLE);
    std::string diffLabel = LS(StringID::OPT_DIFFICULTY);
    std::string easy = LS(StringID::OPT_EASY);
    std::string normal = LS(StringID::OPT_NORMAL);
    std::string hard = LS(StringID::OPT_HARD);
    std::string soundLabel = LS(StringID::OPT_SOUND_FX);
    std::string musicLabel = LS(StringID::OPT_MUSIC);
    std::string volLabel = LS(StringID::OPT_VOLUME);
    std::string langLabel = LS(StringID::OPT_LANGUAGE);
    std::string english = LS(StringID::OPT_ENGLISH);
    std::string spanish = LS(StringID::OPT_SPANISH);
    std::string french = LS(StringID::OPT_FRENCH);
    std::string back = LS(StringID::OPT_BACK);

    drawTitle(title);

    float pw = 400;
    float px = (mScreenW - pw) / 2.0f;
    float py = (mScreenH - 420) / 2.0f;
    float rowW = pw - 20;
    float rowH = 36;
    float rowX = px + 10;
    float startY = py + 80;
    float gap = 48;

    std::vector<std::string> diffOpts = { easy, normal, hard };
    drawSelector(diffLabel, diffOpts, mDifficulty, rowX, startY, rowW, rowH);

    // Sound FX toggle + volume slider
    drawToggle(soundLabel, mSoundOn, rowX, startY + gap, rowW * 0.6f, rowH);
    drawSlider(volLabel, mSoundVolume, rowX, startY + gap + 40, rowW, rowH - 10);

    // Music toggle + volume slider
    drawToggle(musicLabel, mMusicOn, rowX, startY + gap * 2 + 20, rowW * 0.6f, rowH);
    drawSlider(volLabel, mMusicVolume, rowX, startY + gap * 2 + 60, rowW, rowH - 10);

    // Language selector
    std::vector<std::string> langOpts = { english, spanish, french };
    drawSelector(langLabel, langOpts, mLanguage, rowX, startY + gap * 3 + 50, rowW, rowH);

    // Back button
    float bw = 200, bh = 40;
    float bx = px + (pw - bw) / 2.0f;
    if (drawButton(back, bx, py + 380, bw, bh, false))
    {
        fireOptionsEvents();
        mState = (mPreviousState == MenuState::PAUSE_MENU) ? MenuState::PAUSE_MENU : MenuState::MAIN_MENU;
    }
}

void MenuManager::drawPauseMenu()
{
    drawBackground();

    std::string title = LS(StringID::PAUSE_TITLE);
    std::string resume = LS(StringID::PAUSE_RESUME);
    std::string options = LS(StringID::PAUSE_OPTIONS);
    std::string quitMain = LS(StringID::PAUSE_QUIT_TO_MAIN);

    drawTitle(title);

    float pw = 400;
    float px = (mScreenW - pw) / 2.0f;
    float py = (mScreenH - 420) / 2.0f;
    float bw = 280, bh = 44;
    float bx = px + (pw - bw) / 2.0f;
    float startY = py + 90;
    float gap = 56;

    if (drawButton(resume, bx, startY, bw, bh, false))
        EventSystem::getInstance()->fireEvent(MenuResumeEvent());

    if (drawButton(options, bx, startY + gap, bw, bh, false))
    {
        mPreviousState = MenuState::PAUSE_MENU;
        EventSystem::getInstance()->fireEvent(MenuOptionsEvent());
    }

    if (drawButton(quitMain, bx, startY + gap * 2, bw, bh, false))
        EventSystem::getInstance()->fireEvent(MenuQuitToMainEvent());
}

void MenuManager::update(double dtMs)
{
    SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mMouseX, &mMouseY);
    bool isDown = (buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0;

    mMousePressed = isDown;
    if (!isDown) mMouseWasDown = false;
}

void MenuManager::draw()
{
    if (mState == MenuState::NONE) return;

    switch (mState)
    {
    case MenuState::MAIN_MENU: drawMainMenu(); break;
    case MenuState::OPTIONS_MENU: drawOptionsMenu(); break;
    case MenuState::PAUSE_MENU: drawPauseMenu(); break;
    default: break;
    }

    if (mMousePressed) mMouseWasDown = true;
}

void MenuManager::handleEvent(const Event& theEvent)
{
    switch (theEvent.getType())
    {
    case MENU_PLAY_EVENT:
        std::cout << "MenuManager: Play New Game\n";
        mState = MenuState::NONE;
        break;
    case MENU_LOAD_EVENT:
        std::cout << "MenuManager: Load Game (stub)\n";
        mState = MenuState::NONE;
        break;
    case MENU_OPTIONS_EVENT:
        mPreviousState = mState;
        mState = MenuState::OPTIONS_MENU;
        break;

    case MENU_QUIT_EVENT:
        std::cout << "MenuManager: Quit\n";
        EventSystem::getInstance()->fireEvent(GameEvent(QUIT_EVENT));
        break;
    case MENU_RESUME_EVENT:
        mState = MenuState::NONE;
        break;
    case MENU_QUIT_TO_MAIN_EVENT:
        if (mpFileManager)
            mpFileManager->saveSettings("Settings.txt",
                mSoundOn, mSoundVolume,
                mMusicOn, mMusicVolume,
                mLanguage);
        std::cout << "MenuManager: Quit to Main Menu\n";
        mState = MenuState::MAIN_MENU;
        break;
    case MENU_PAUSE_EVENT:
        if (mState == MenuState::NONE)
            mState = MenuState::PAUSE_MENU;
        break;
    default:
        break;
    }
}


/*

 //https://discourse.libsdl.org/t/sdl3-menu-on-windows-mac-and-linux/65503
 //https://wiki.libsdl.org/SDL3/SDL_ShowWindowSystemMenu
 //https://gamedev.stackexchange.com/questions/72878/how-can-i-implement-a-main-menu
 //https://wiki.libsdl.org/SDL3/SDL_HINT_WINDOWS_ENABLE_MENU_MNEMONICS
 //https://stackoverflow.com/questions/1852655/how-to-create-an-sdl-pop-up-menu
 //https://www.youtube.com/watch?v=k-m0q_uVnno
 //https://wiki.libsdl.org/SDL3/SDL_main
 //https://wiki.libsdl.org/SDL3/SDL_CreateWindow
 //https://www.studyplan.dev/sdl3/sdl3-window
 //https://wiki.libsdl.org/SDL3/SDL_TrayMenu
 //https://wiki.libsdl.org/SDL3/SDL_HINT_VIDEO_MAC_FULLSCREEN_MENU_VISIBILITY
 //https://courses.mshah.io/courses/sdl3
 //https://wiki.libsdl.org/SDL3/SDL_PauseAudioDevice
 //https://wiki.libsdl.org/SDL3/SDL_PauseAudioStreamDevice
 //https://wiki.libsdl.org/SDL3/SDL_CPUPauseInstruction
 //https://wiki.libsdl.org/SDL3/SDL_CPUPauseInstruction
 //https://wiki.libsdl.org/SDL3/SDL_PauseHaptic
 //https://discourse.libsdl.org/t/pausing-code/3551
 //https://stackoverflow.com/questions/8152908/sdl-how-to-stop-audio-not-resume-which-sdl-pauseaudio1-does-actually
 //https://www.studyplan.dev/sdl3/sdl3-display-modes
 //https://stackoverflow.com/questions/79683348/how-to-make-config-cmake-file-of-sdl3-and-imgui
 //https://www.studyplan.dev/sdl3/sdl3-window-configuration
 //https://wiki.libsdl.org/SDL3/SDL_GL_SetAttribute
 //https://wiki.libsdl.org/SDL3/QuickReference
 //https://wiki.libsdl.org/SDL3/Libraries
 //https://www.studyplan.dev/sdl3/sdl3-ui
 //https://lib.rs/crates/imgui-sdl3

*/