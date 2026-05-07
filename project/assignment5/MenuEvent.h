#pragma once
#include "GameEvent.h"

class MenuPlayEvent : public GameEvent
{
public:
	MenuPlayEvent() : GameEvent(MENU_PLAY_EVENT) {}
	~MenuPlayEvent() {}
};

class MenuLoadEvent : public GameEvent
{
public:
    MenuLoadEvent() : GameEvent(MENU_LOAD_EVENT) {}
    ~MenuLoadEvent() {}
};

class MenuOptionsEvent : public GameEvent
{
public:
    MenuOptionsEvent() : GameEvent(MENU_OPTIONS_EVENT) {}
    ~MenuOptionsEvent() {}
};

class MenuQuitEvent : public GameEvent
{
public:
    MenuQuitEvent() : GameEvent(MENU_QUIT_EVENT) {}
    ~MenuQuitEvent() {}
};

class MenuResumeEvent : public GameEvent
{
public:
    MenuResumeEvent() : GameEvent(MENU_RESUME_EVENT) {}
    ~MenuResumeEvent() {}
};

class MenuQuitToMainEvent : public GameEvent
{
public:
    MenuQuitToMainEvent() : GameEvent(MENU_QUIT_TO_MAIN_EVENT) {}
    ~MenuQuitToMainEvent() {}
};

class MenuPauseEvent : public GameEvent
{
public:
    MenuPauseEvent() : GameEvent(MENU_PAUSE_EVENT) {}
    ~MenuPauseEvent() {}
};

class MenuDifficultyEvent : public GameEvent
{
public:
    MenuDifficultyEvent(int difficulty) : GameEvent(MENU_DIFFICULTY_EVENT), mDifficulty(difficulty) {}
    ~MenuDifficultyEvent() {}
    int getDifficulty() const { return mDifficulty; }
private:
    int mDifficulty = 1;
};

class MenuSoundEvent : public GameEvent
{
public:
    MenuSoundEvent(bool on, int volume) : GameEvent(MENU_SOUND_EVENT), mOn(on), mVolume(volume) {}
    ~MenuSoundEvent() {}
    bool isOn()    const { return mOn; }
    int  getVolume() const { return mVolume; }
private:
    bool mOn = true;
    int  mVolume = 100;
};

class MenuMusicEvent : public GameEvent
{
public:
    MenuMusicEvent(bool on, int volume) : GameEvent(MENU_MUSIC_EVENT), mOn(on), mVolume(volume) {}
    ~MenuMusicEvent() {}
    bool isOn()    const { return mOn; }
    int  getVolume() const { return mVolume; }
private:
    bool mOn = true;
    int  mVolume = 100;
};

class MenuLanguageEvent : public GameEvent
{
public:
    MenuLanguageEvent(int lang) : GameEvent(MENU_LANGUAGE_EVENT), mLanguage(lang) {}
    ~MenuLanguageEvent() {}
    int getLanguage() const { return mLanguage; }
private:
    int mLanguage = 0;
};
