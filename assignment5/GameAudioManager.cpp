#include "GameAudioManager.h"
#include "EventSystem.h"
#include "GameEvent.h"
#include "MenuEvent.h"
#include "WebEvent.h"
#include "BossEvent.h"
#include "ConsumingEvent.h"
#include <iostream>
#include "FileManager.h"

GameAudioManager* GameAudioManager::mpsInstance = nullptr;

GameAudioManager* GameAudioManager::createInstance()
{
    if (mpsInstance == nullptr)
        mpsInstance = new GameAudioManager;
    return mpsInstance;
}

void GameAudioManager::destroyInstance()
{
    delete mpsInstance;
    mpsInstance = nullptr;
}

bool GameAudioManager::gameInit(const std::string& assetPath)
{
    FileManager::getInstance()->readAndImportFile(DATA_FILE);
    mAssetPath = assetPath;

    if (!AudioManager::init())
        return false;


    auto load = [&](const SoundKey& key, const std::string& file)
        {
            loadSound(key, mAssetPath + file);
        };

    load("bgm_menu", "bgm_menu.wav");
    load("bgm_level", "bgm_level.wav");
    load("bgm_boss", "bgm_boss.wav");
    load("sfx_web_shoot", "sfx_web_shoot.wav");
    load("sfx_web_blast", "sfx_web_blast.wav");
    load("sfx_player_damage", "sfx_player_damage.wav");
    load("sfx_player_block", "sfx_player_block.wav");
    load("sfx_player_death", "sfx_player_death.wav");
    load("sfx_enemy_attack", "sfx_enemy_attack.wav");
    load("sfx_enemy_death", "sfx_enemy_death.wav");
    load("sfx_boss_attack", "sfx_boss_attack.wav");
    load("sfx_boss_hurt", "sfx_boss_hurt.wav");
    load("sfx_boss_death", "sfx_boss_death.wav");
    load("sfx_boss_phase", "sfx_boss_phase.wav");
    load("sfx_menu_select", "sfx_menu_select.wav");
    load("sfx_menu_confirm", "sfx_menu_confirm.wav");
    load("sfx_level_win", "sfx_level_win.wav");
    load("sfx_game_over", "sfx_game_over.wav");
    auto* pES = EventSystem::getInstance();
    pES->addListener((EventType)WEB_FIRE_EVENT, this);
    pES->addListener((EventType)WEB_BLAST_EVENT, this);
    pES->addListener((EventType)WEB_HIT_EVENT, this);
    pES->addListener((EventType)BLOCK_EVENT, this);
    pES->addListener((EventType)BOSS_ATTACK_EVENT, this);
    pES->addListener((EventType)BOSS_PHASE_EVENT, this);
    pES->addListener((EventType)BOSS_DEAD_EVENT, this);
    pES->addListener((EventType)MENU_PLAY_EVENT, this);
    pES->addListener((EventType)MENU_PAUSE_EVENT, this);
    pES->addListener((EventType)MENU_RESUME_EVENT, this);
    pES->addListener((EventType)MENU_QUIT_EVENT, this);
    pES->addListener((EventType)MENU_OPTIONS_EVENT, this);
    pES->addListener((EventType)MENU_QUIT_TO_MAIN_EVENT, this);
    pES->addListener((EventType)MENU_SOUND_EVENT, this);
    pES->addListener((EventType)MENU_MUSIC_EVENT, this);
    pES->addListener((EventType)MENU_DIFFICULTY_EVENT, this);
    pES->addListener((EventType)SCORE_EVENT, this);

    std::cout << "GameAudioManager: initialized\n";
    return true;
}

void GameAudioManager::gameCleanup()
{
    stopMusic();
}


void GameAudioManager::gameUpdate(double dtMs)
{
    float dt = (float)dtMs;
    auto tick = [&](float& t) { if (t > 0.0f) t -= dt; };
    tick(mCdWebShot);
    tick(mCdWebBlast);
    tick(mCdPlayerDamage);
    tick(mCdPlayerBlock);
    tick(mCdEnemyAttack);
    tick(mCdEnemyDeath);
    tick(mCdBossAttack);
    tick(mCdBossHurt);
    tick(mCdMenuSelect);

    tickMusicLoop(dtMs);

    AudioManager::update(dtMs);
}


void GameAudioManager::setSoundEnabled(bool on)
{
    mSoundEnabled = on;
    std::cout << "GameAudioManager: sound " << (on ? "ON" : "OFF") << "\n";
}

void GameAudioManager::setMusicEnabled(bool on)
{
    mMusicEnabled = on;
    if (!on)
        stopMusic();
    else if (mCurrentTrack != MusicTrack::NONE)
        startMusicStream(mMusicKey); 
    std::cout << "GameAudioManager: music " << (on ? "ON" : "OFF") << "\n";
}

void GameAudioManager::setSoundVolume(float vol01)
{
    mSoundVol = (vol01 < 0.0f) ? 0.0f : (vol01 > 1.0f) ? 1.0f : vol01;
}

void GameAudioManager::setMusicVolume(float vol01)
{
    mMusicVol = (vol01 < 0.0f) ? 0.0f : (vol01 > 1.0f) ? 1.0f : vol01;
    if (mpMusicStream)
        SDL_SetAudioStreamGain(mpMusicStream, mMusicVol);
}

void GameAudioManager::playMusic(MusicTrack track)
{
    if (track == mCurrentTrack) return;

    stopMusic();
    mCurrentTrack = track;

    switch (track)
    {
    case MusicTrack::MENU:  mMusicKey = "bgm_menu";  break;
    case MusicTrack::LEVEL: mMusicKey = "bgm_level"; break;
    case MusicTrack::BOSS:  mMusicKey = "bgm_boss";  break;
    default: return;
    }

    if (mMusicEnabled)
        startMusicStream(mMusicKey);
}

void GameAudioManager::stopMusic()
{
    if (mpMusicStream)
    {
        SDL_DestroyAudioStream(mpMusicStream);
        mpMusicStream = nullptr;
    }
    mMusicPlayedMs = 0.0f;
    mMusicLengthMs = 0.0f;
}

void GameAudioManager::startMusicStream(const SoundKey& key)
{
    stopMusic();

    SDL_AudioSpec spec;
    Uint8* buf = nullptr;
    Uint32  length = 0;

    std::string path = mAssetPath + key + ".wav";

    if (!SDL_LoadWAV(path.c_str(), &spec, &buf, &length))
    {
        std::cout << "GameAudioManager: music load failed (" << path << "): " << SDL_GetError() << "\n";
        return;
    }

    mpMusicStream = SDL_CreateAudioStream(&spec, &spec);
    if (!mpMusicStream)
    {
        SDL_free(buf);
        return;
    }

    SDL_SetAudioStreamGain(mpMusicStream, mMusicVol);
    SDL_PutAudioStreamData(mpMusicStream, buf, (int)length);
    SDL_FlushAudioStream(mpMusicStream);
    SDL_BindAudioStream(mDeviceID, mpMusicStream);
    int bytesPerSample = SDL_AUDIO_BYTESIZE(spec.format);
    int bytesPerFrame = bytesPerSample * spec.channels;
    if (spec.freq > 0 && bytesPerFrame > 0)
        mMusicLengthMs = (float)length / (float)bytesPerFrame / (float)spec.freq * 1000.0f;

    mMusicPlayedMs = 0.0f;
    SDL_free(buf);   

    std::cout << "GameAudioManager: playing music \"" << key << "\" (" << (int)mMusicLengthMs << "ms)\n";
}

void GameAudioManager::tickMusicLoop(double dtMs)
{
    if (!mpMusicStream || mMusicLengthMs <= 0.0f) return;

    mMusicPlayedMs += (float)dtMs;
    if (mMusicPlayedMs >= mMusicLengthMs)
    {
        startMusicStream(mMusicKey);
    }
}

void GameAudioManager::playSfx(const SoundKey& key, float& cooldownTimer, float cooldownMs)
{
    if (!mSoundEnabled) return;
    if (cooldownTimer > 0.0f) return;
    playSound(key);
    cooldownTimer = cooldownMs;
}

void GameAudioManager::playWebShot()
{
    playSfx("sfx_web_shoot", mCdWebShot, CD_WEB_SHOT);
}
void GameAudioManager::playWebBlast()
{
    playSfx("sfx_web_blast", mCdWebBlast, CD_WEB_BLAST);
}
void GameAudioManager::playPlayerDamage()
{
    playSfx("sfx_player_damage", mCdPlayerDamage, CD_PLAYER_DAMAGE);
}
void GameAudioManager::playPlayerBlock()
{
    playSfx("sfx_player_block", mCdPlayerBlock, CD_PLAYER_BLOCK);
}
void GameAudioManager::playPlayerDeath()
{
    if (mSoundEnabled) playSound("sfx_player_death");
}
void GameAudioManager::playEnemyAttack()
{
    playSfx("sfx_enemy_attack", mCdEnemyAttack, CD_ENEMY_ATTACK);
}
void GameAudioManager::playEnemyDeath()
{
    playSfx("sfx_enemy_death", mCdEnemyDeath, CD_ENEMY_DEATH);
}
void GameAudioManager::playBossAttack()
{
    playSfx("sfx_boss_attack", mCdBossAttack, CD_BOSS_ATTACK);
}
void GameAudioManager::playBossHurt()
{
    playSfx("sfx_boss_hurt", mCdBossHurt, CD_BOSS_HURT);
}
void GameAudioManager::playBossDeath()
{
    if (mSoundEnabled) playSound("sfx_boss_death");
}
void GameAudioManager::playBossPhaseChange()
{
    if (mSoundEnabled) playSound("sfx_boss_phase");
}
void GameAudioManager::playMenuSelect()
{
    playSfx("sfx_menu_select", mCdMenuSelect, CD_MENU_SELECT);
}
void GameAudioManager::playMenuConfirm()
{
    if (mSoundEnabled) playSound("sfx_menu_confirm");
}
void GameAudioManager::playLevelWin()
{
    if (mSoundEnabled) playSound("sfx_level_win");
}
void GameAudioManager::playGameOver()
{
    if (mSoundEnabled) playSound("sfx_game_over");
}

void GameAudioManager::handleEvent(const Event& theEvent)
{
    AudioManager::handleEvent(theEvent);

    switch (theEvent.getType())
    {
    case WEB_FIRE_EVENT:
        playWebShot();
        break;

    case WEB_BLAST_EVENT:
        playWebBlast();
        break;

    case WEB_HIT_EVENT:
        playSfx("sfx_enemy_attack", mCdEnemyAttack, CD_ENEMY_ATTACK);
        break;

    case BLOCK_EVENT:
    {
        const BlockEvent& e = static_cast<const BlockEvent&>(theEvent);
        if (e.isBlocking())
            playPlayerBlock();
        break;
    }

    case BOSS_ATTACK_EVENT:
        playBossAttack();
        break;

    case BOSS_PHASE_EVENT:
        playBossPhaseChange();
        playMusic(MusicTrack::BOSS);
        break;

    case BOSS_DEAD_EVENT:
        playBossDeath();
        stopMusic();
        playLevelWin();
        break;

    case SCORE_EVENT:
        playEnemyDeath();
        break;

    case MENU_PLAY_EVENT:
        playMenuConfirm();
        playMusic(MusicTrack::LEVEL);
        break;

    case MENU_OPTIONS_EVENT:
    case MENU_DIFFICULTY_EVENT:
        playMenuSelect();
        break;

    case MENU_PAUSE_EVENT:
        playMenuSelect();
        setMusicVolume(mMusicVol * 0.4f);
        break;

    case MENU_RESUME_EVENT:
        playMenuSelect();
        setMusicVolume(mMusicVol / 0.4f > 1.0f ? 1.0f : mMusicVol / 0.4f);
        break;

    case MENU_QUIT_TO_MAIN_EVENT:
        playMenuConfirm();
        playMusic(MusicTrack::MENU);
        break;

    case MENU_QUIT_EVENT:
        playMenuConfirm();
        break;
    case MENU_SOUND_EVENT:
    {
        const MenuSoundEvent& e = static_cast<const MenuSoundEvent&>(theEvent);
        setSoundEnabled(e.isOn());
        setSoundVolume((float)e.getVolume() / 100.0f);
        break;
    }
    case MENU_MUSIC_EVENT:
    {
        const MenuMusicEvent& e = static_cast<const MenuMusicEvent&>(theEvent);
        setMusicEnabled(e.isOn());
        setMusicVolume((float)e.getVolume() / 100.0f);
        break;
    }

    default:
        break;
    }
}
