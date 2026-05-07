#pragma once
#include "AudioManager.h"
#include "GameEvent.h"
#include <string>

class GameAudioManager : public AudioManager
{
    friend class FileManager;
public:
    static GameAudioManager* createInstance();
    static GameAudioManager* getInstance() { return mpsInstance; };
    static void destroyInstance();

    enum class MusicTrack { NONE, MENU, LEVEL, BOSS };
    
    bool gameInit(const std::string& assetPath);
    void gameCleanup();
    void gameUpdate(double dtMs);
    void setSoundEnabled(bool on);
    void setMusicEnabled(bool on);
    void setSoundVolume(float vol01);   
    void setMusicVolume(float vol01);
    bool isSoundEnabled() const { return mSoundEnabled; }
    bool isMusicEnabled() const { return mMusicEnabled; }
    float getSoundVolume() const { return mSoundVol; }
    float getMusicVolume() const { return mMusicVol; }
    void playMusic(MusicTrack track);
    void stopMusic();
    void playWebShot();
    void playWebBlast();
    void playPlayerDamage();
    void playPlayerBlock();
    void playPlayerDeath();
    void playEnemyAttack();
    void playEnemyDeath();
    void playBossAttack();
    void playBossHurt();
    void playBossDeath();
    void playBossPhaseChange();
    void playMenuSelect();
    void playMenuConfirm();
    void playLevelWin();
    void playGameOver();
    void handleEvent(const Event& theEvent) override;

private:
    static GameAudioManager* mpsInstance;
    GameAudioManager() {}
    ~GameAudioManager() { gameCleanup(); }

    void playSfx(const SoundKey& key, float& cooldownTimer, float cooldownMs = 0.0f);
    void startMusicStream(const SoundKey& key);
    void tickMusicLoop(double dtMs);
    std::string mAssetPath;
    MusicTrack mCurrentTrack = MusicTrack::NONE;
    SDL_AudioStream* mpMusicStream = nullptr;
    SoundKey mMusicKey;  
    bool mSoundEnabled = true;
    bool mMusicEnabled = true;
    float mSoundVol = 1.0f;  
    float mMusicVol = 0.6f;
    float mCdWebShot = 0.0f;
    float mCdWebBlast = 0.0f;
    float mCdPlayerDamage = 0.0f;
    float mCdPlayerBlock = 0.0f;
    float mCdEnemyAttack = 0.0f;
    float mCdEnemyDeath = 0.0f;
    float mCdBossAttack = 0.0f;
    float mCdBossHurt = 0.0f;
    float mCdMenuSelect = 0.0f;
    float mMusicPlayedMs = 0.0f;
    float mMusicLengthMs = 0.0f;

    inline void setWebShotSoundCooldown(float cd) { CD_WEB_SHOT = cd; };
    inline void setWebBlastSoundCooldown(float cd) { CD_WEB_BLAST = cd; };
    inline void setPlayerDamageSoundCooldown(float cd) { CD_PLAYER_DAMAGE = cd; };
    inline void setPlayerBlockSoundCooldown(float cd) { CD_PLAYER_BLOCK = cd; };
    inline void setEnemyAttackSoundCooldown(float cd) { CD_ENEMY_ATTACK = cd; };
    inline void setEnemyDeathSoundCooldown(float cd) { CD_ENEMY_DEATH = cd; };
    inline void setBossAttackSoundCooldown(float cd) { CD_BOSS_ATTACK = cd; };
    inline void setBossHurtSoundCooldown(float cd) { CD_BOSS_HURT = cd; };
    inline void setMenuSelectSoundCooldown(float cd) { CD_MENU_SELECT = cd; };

    const std::string DATA_FILE = "GameAudioManagerData.txt";

    // THESE ARE DATA DRIVEN - CHANGE FILE VALUES NOT THESE | values here are fallback if file cannot be read
    float CD_WEB_SHOT = 80.0f;
    float CD_WEB_BLAST = 300.0f;
    float CD_PLAYER_DAMAGE = 200.0f;
    float CD_PLAYER_BLOCK = 150.0f;
    float CD_ENEMY_ATTACK = 100.0f;
    float CD_ENEMY_DEATH = 80.0f;
    float CD_BOSS_ATTACK = 200.0f;
    float CD_BOSS_HURT = 150.0f;
    float CD_MENU_SELECT = 80.0f;
};
