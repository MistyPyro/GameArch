#pragma once
#include "EventListener.h"
#include "Color.h"
#include "Vector2D.h"
#include <string>
#include <vector>

class Font;
class GraphicsBuffer;
class FileManager;

class MenuManager : public EventListener
{
public:
    enum class MenuState
    {
        MAIN_MENU,
        OPTIONS_MENU,
        PAUSE_MENU,
        NONE      
    };

    MenuManager();
    ~MenuManager() { cleanup(); }
    void init(Font* pFont, unsigned int screenW, unsigned int screenH);
    void cleanup();
    void update(double dtMs);
    void draw();
    bool isGameRunning() const { return mState == MenuState::NONE; }
    bool isPaused() const { return mState == MenuState::PAUSE_MENU; }
    MenuState getState() const { return mState; }
    void setState(MenuState s) { mState = s; }
    int  getDifficulty() const { return mDifficulty; }
    bool isSoundOn() const { return mSoundOn; }
    int  getSoundVolume() const { return mSoundVolume; }
    bool isMusicOn() const { return mMusicOn; }
    int  getMusicVolume() const { return mMusicVolume; }
    int  getLanguage() const { return mLanguage; }

    void setFileManager(FileManager* pFileManager) { mpFileManager = pFileManager; }
    void handleEvent(const Event& theEvent);

private:
    void drawMainMenu();
    void drawOptionsMenu();
    void drawPauseMenu();
    void drawBackground();
    void drawTitle(const std::string& title);
    bool drawButton(const std::string& label, float x, float y, float w, float h, bool selected);

    void drawToggle(const std::string& label, bool& value, float x, float y, float w, float h);

    void drawSlider(const std::string& label, int& value, float x, float y, float w, float h);

    void drawSelector(const std::string& label, const std::vector<std::string>& options, int& selected, float x, float y, float w, float h);

    void fireOptionsEvents();
    MenuState    mState = MenuState::MAIN_MENU;
    MenuState    mPreviousState = MenuState::MAIN_MENU;
    Font* mpFont = nullptr;
    FileManager* mpFileManager = nullptr;
    unsigned int mScreenW = 800;
    unsigned int mScreenH = 600;
    float mMouseX = 0;
    float mMouseY = 0;
    bool mMousePressed = false;
    bool mMouseWasDown = false;
    int mDifficulty = 1;  
    bool mSoundOn = true;
    int mSoundVolume = 100;
    bool mMusicOn = true;
    int mMusicVolume = 100;
    int mLanguage = 0;  

    // Colours
    const Color COL_OVERLAY = Color(0, 0, 0, 180);
    const Color COL_PANEL = Color(30, 30, 30, 220);
    const Color COL_BTN = Color(60, 60, 80, 255);
    const Color COL_BTN_HOVER = Color(90, 90, 130, 255);
    const Color COL_BTN_SEL = Color(100, 60, 160, 255);
    const Color COL_TEXT = Color(255, 255, 255, 255);
    const Color COL_TITLE = Color(255, 220, 50, 255);
    const Color COL_SLIDER_BG = Color(50, 50, 50, 255);
    const Color COL_SLIDER_FILL = Color(100, 60, 200, 255);
};