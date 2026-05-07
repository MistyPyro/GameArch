// Game.cpp  – SDL3 port (no Allegro)
#include "Game.h"
#include "PerformanceTracker.h"
#include <MemoryTracker.h>
#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include "Color.h"
#include "Sprite.h"
#include "InputSystem.h"
#include "Animation.h"
#include "Unit.h"
#include "UnitManager.h"
#include "GraphicsBufferManager.h"
#include "Font.h"
#include "GameEvent.h"
#include "EventSystem.h"
#include "InputTranslator.h"
#include "SpawnEvent.h"
#include "DestroyEvent.h"
#include "AnimationManager.h"
#include "AudioManager.h"
#include "ConsumingEvent.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "HUD.h"
#include "ScoreEvent.h"
#include "MenuManager.h"
#include "MenuEvent.h"
#include "WebManager.h"
#include "WebEvent.h"
#include "FileManager.h"
#include "Camera.h"
#include "ScrollingBackground.h"
#include "BossManager.h"
#include "BossEvent.h"
#include "DeathScreen.h"
#include "PhysicsManager.h"
#include "WinScreen.h"
#include "LevelManager.h"
#include "SaveManager.h"
#include "GameAudioManager.h"

Game* Game::mpsInstance = nullptr;
using namespace std;

Game* Game::createInstance()
{
    if (mpsInstance == nullptr)
        mpsInstance = new Game;
    return mpsInstance;
}

void Game::destroyInstance()
{
    delete mpsInstance;
    mpsInstance = nullptr;
}

Game::~Game()
{
    cleanup();
}


void Game::init()
{
    mpFileManager = FileManager::createInstance();
    mpFileManager->readAndImportFile(DATA_FILE);
    SAVE_FILE = GAME_ASSET_PATH + "SaveGame.txt";

    mpGraphicsSystem = new GraphicsSystem;
    mpInputSystem = new InputSystem();
    mpUnitManager = new UnitManager(WIDTH, HEIGHT);
    mpGraphicsBufferManager = new GraphicsBufferManager();

    if (mpGraphicsSystem)
        mpGraphicsSystem->cleanup();

    mRet = mpGraphicsSystem->init(WIDTH, HEIGHT);
    mpInputSystem->init();

    mpFont = new Font(ASSET_PATH + FONT_FILENAME, FONT_SIZE);

    EventSystem::getInstance()->addListener((EventType)QUIT_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)SPAWN_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)DESTROY_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)DESTROY_ALL_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)SWITCH_ANIMATION_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)FREEZE_ANIMATION_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)COLLISION_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_PLAY_EVENT, this);
    EventSystem::getInstance()->addListener((EventType)MENU_LOAD_EVENT, this);

    mpAnimationManager = new AnimationManager();

    mpAudioManager = GameAudioManager::createInstance();
    mpAudioManager->gameInit(GAME_ASSET_PATH);

    mpFileManager->loadSettings("Settings.txt");

    // Legacy sounds used by the base AudioManager for SPAWN/DESTROY/CONSUMING events
  //  mpAudioManager->loadSound("spawn", AUDIO_ASSET_PATH + "BLIP.WAV");
  //  mpAudioManager->loadSound("switch", AUDIO_ASSET_PATH + "BLIP.WAV");
   // mpAudioManager->loadSound("destroy", AUDIO_ASSET_PATH + "BLIP.WAV");
  //  mpAudioManager->loadSound("consuming", AUDIO_ASSET_PATH + "KILLED.WAV");
  //  mpAudioManager->loadSound("Eat", GAME_ASSET_PATH + "MinecraftEattingSoundSpeedUp.wav");

    mpInputTranslator = new InputTranslator;

    // sprite/animation constants
    const unsigned int FADVENTURER_FILE_NUM = 8;
    const unsigned int FPERSON_WIDTH = 96;
    const unsigned int FPERSON_HEIGHT = 128;
    const unsigned int SPIDER_EAT_SPRITES_ACROSS = 2;
    const unsigned int SPIDER_EAT_SPRITES_DOWN = 3;
    const unsigned int SPIDER_EAT_WIDTH = 132;
    const unsigned int SPIDER_EAT_HEIGHT = 132;
    const unsigned int SPIDER_IDLE_ACROSS = 4;
    const unsigned int SPIDER_IDLE_DOWN = 1;
    const unsigned int SPIDER_IDLE_WIDTH = 132;
    const unsigned int SPIDER_IDLE_Y_OFFSET = 80;
    const unsigned int SPIDER_IDLE_HEIGHT = 132;
    const unsigned int SPIDER_SPRITE_SIZE = 132; 
    const unsigned int SPIDER_SHOOT_ACROSS = 2;
    const unsigned int SPIDER_SHOOT_DOWN = 1;
    const unsigned int SPIDER_BLAST_ACROSS = 4;
    const unsigned int SPIDER_BLAST_DOWN = 1;
    const unsigned int SPIDER_BLOCK_ACROSS = 1;
    const unsigned int SPIDER_BLOCK_DOWN = 1;
    const unsigned int SPIDER_PUNCH_ACROSS = 4;
    const unsigned int SPIDER_PUNCH_DOWN = 2;
    const unsigned int SPIDER_DEATH_ACROSS = 2;
    const unsigned int SPIDER_DEATH_DOWN = 4;
    const unsigned int ENEMY_WALK_ACROSS = 4;
    const unsigned int ENEMY_WALK_DOWN = 4;
    const unsigned int ENEMY_WALK_SIZE = 132;
    const unsigned int ENEMY_ATTACK_ACROSS = 5;
    const unsigned int ENEMY_ATTACK_DOWN = 1;
    const unsigned int ENEMY_ATTACK_SIZE = 132;
    const unsigned int ENEMY_DIE_ACROSS = 3;
    const unsigned int ENEMY_DIE_DOWN = 3;
    const unsigned int ENEMY_DIE_SIZE = 132;

    // background
    mpGraphicsBufferManager->createAndManageGraphicsBuffer("Background", GAME_ASSET_PATH + BG_FILENAME);

    mpGraphicsBufferManager->createAndManageGraphicsBuffer("Background2", GAME_ASSET_PATH + "backgroundLevel2.png");

    //fallback / legacy animations (used by UnitManager)
    std::vector<Sprite> tempSprite;

    mpGraphicsBufferManager->createAndManageGraphicsBuffer("SpidermanIdle", GAME_ASSET_PATH + "SpidermanSpriteIdle.png");
    for (unsigned int y = 0; y < SPIDER_IDLE_DOWN; y++)
        for (unsigned int x = 0; x < SPIDER_IDLE_ACROSS; x++)
        tempSprite.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("SpidermanIdle"),
            Vector2D((float)(x * SPIDER_IDLE_WIDTH), (float)( y * SPIDER_IDLE_HEIGHT)),
            SPIDER_IDLE_WIDTH, SPIDER_IDLE_HEIGHT));
    mpAnimationManager->createAndAddAnimation(true, tempSprite, "SpidermanIdle");
    tempSprite.clear();

    mpGraphicsBufferManager->createAndManageGraphicsBuffer("SpidermanEating", GAME_ASSET_PATH + "SpidermanWalkingSheet.png");
    for (unsigned int y = 0; y < SPIDER_EAT_SPRITES_DOWN; y++)
        for (unsigned int x = 0; x < SPIDER_EAT_SPRITES_ACROSS; x++)
            tempSprite.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("SpidermanEating"),
                Vector2D((float)(x * SPIDER_EAT_WIDTH), (float)(y * SPIDER_EAT_HEIGHT)),
                SPIDER_EAT_WIDTH, SPIDER_EAT_HEIGHT));
    mpAnimationManager->createAndAddAnimation(true, tempSprite, "SpidermanEating");
    tempSprite.clear();

    for (unsigned int x = 0; x < FADVENTURER_FILE_NUM; x++)
    {
        mpGraphicsBufferManager->createAndManageGraphicsBuffer(
            "FAdventurer" + std::to_string(x),
            ASSET_PATH + KENNEY_ASSET_PATH + FPERSON_FILENAME + std::to_string(x) + ".png");
        tempSprite.push_back(Sprite(
            mpGraphicsBufferManager->getGraphicsBuffer("FAdventurer" + std::to_string(x)),
            Vector2D(0, 0), FPERSON_WIDTH, FPERSON_HEIGHT));
    }
    mpAnimationManager->createAndAddAnimation(true, tempSprite, "fAdventurerWalk");
    tempSprite.clear();

    mAnimations.push_back(*mpAnimationManager->getAnimation("SpidermanIdle"));
    mAnimations.push_back(*mpAnimationManager->getAnimation("SpidermanEating"));
    mAnimations.push_back(*mpAnimationManager->getAnimation("fAdventurerWalk"));

    //physics + player
    mpPhysicsManager = PhysicsManager::createInstance();

    mpPlayerManager = PlayerManager::createInstance();
    mpPlayerManager->init();
    mpPlayerManager->createPlayer(mAnimations,Vector2D((float)PLAYER_SPAWN_X, (float)PLAYER_SPAWN_Y), UNIT_SPEED);
    mpInputTranslator->setPlayerManager(mpPlayerManager);
    mpPlayerManager->setAudioManager(mpAudioManager);

    //player animations
    std::vector<Sprite> ss; // reusable sprite scratch vector

    // Idle
    for (unsigned int y = 0; y < SPIDER_IDLE_DOWN; y++)
        for (unsigned int x = 0; x < SPIDER_IDLE_ACROSS; x++)
            ss.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("SpidermanIdle"),
                Vector2D((float)(x * SPIDER_IDLE_WIDTH), (float)(y * SPIDER_IDLE_HEIGHT)),
                SPIDER_IDLE_WIDTH, SPIDER_IDLE_HEIGHT));
        mpAnimationManager->createAndAddAnimation(true, ss, "playerIdle"); ss.clear();

    // Walk
    for (unsigned int y = 0; y < SPIDER_EAT_SPRITES_DOWN; y++)
        for (unsigned int x = 0; x < SPIDER_EAT_SPRITES_ACROSS; x++)
            ss.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("SpidermanEating"),
                Vector2D((float)(x * SPIDER_EAT_WIDTH), (float)(y * SPIDER_EAT_HEIGHT)),
                SPIDER_EAT_WIDTH, SPIDER_EAT_HEIGHT));
    mpAnimationManager->createAndAddAnimation(true, ss, "playerWalk"); ss.clear();

    // Shoot
    mpGraphicsBufferManager->createAndManageGraphicsBuffer("PlayerShoot", GAME_ASSET_PATH + "SpidermanSpriteWebShooting.png");
    for (unsigned int y = 0; y < SPIDER_SHOOT_DOWN; y++)
        for (unsigned int x = 0; x < SPIDER_SHOOT_ACROSS; x++)
            ss.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("PlayerShoot"),
                Vector2D((float)(x * SPIDER_SPRITE_SIZE), (float)(y * SPIDER_SPRITE_SIZE)),
                SPIDER_SPRITE_SIZE, SPIDER_SPRITE_SIZE));
    mpAnimationManager->createAndAddAnimation(false, ss, "playerShoot"); ss.clear();

    // Blast
    mpGraphicsBufferManager->createAndManageGraphicsBuffer("PlayerBlast", GAME_ASSET_PATH + "SpidermanSpriteWebBlast.png");
    for (unsigned int y = 0; y < SPIDER_BLAST_DOWN; y++)
        for (unsigned int x = 0; x < SPIDER_BLAST_ACROSS; x++)
            ss.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("PlayerBlast"),
                Vector2D((float)(x * SPIDER_SPRITE_SIZE), (float)(y * SPIDER_SPRITE_SIZE)),
                SPIDER_SPRITE_SIZE, SPIDER_SPRITE_SIZE));
    mpAnimationManager->createAndAddAnimation(false, ss, "playerBlast"); ss.clear();

    // Block
    mpGraphicsBufferManager->createAndManageGraphicsBuffer("PlayerBlock", GAME_ASSET_PATH + "SpidermanSpriteBlock.png");
    for (unsigned int y = 0; y < SPIDER_BLOCK_DOWN; y++)
        for (unsigned int x = 0; x < SPIDER_BLOCK_ACROSS; x++)
            ss.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("PlayerBlock"),
                Vector2D((float)(x * SPIDER_SPRITE_SIZE), (float)(y * SPIDER_SPRITE_SIZE)),
                SPIDER_SPRITE_SIZE, SPIDER_SPRITE_SIZE));
    mpAnimationManager->createAndAddAnimation(true, ss, "playerBlock"); ss.clear();

    // Punch
    mpGraphicsBufferManager->createAndManageGraphicsBuffer("PlayerPunch", GAME_ASSET_PATH + "SpidermanSpritePunch.png");
    for (unsigned int y = 0; y < SPIDER_PUNCH_DOWN; y++)
        for (unsigned int x = 0; x < SPIDER_PUNCH_ACROSS; x++)
            ss.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("PlayerPunch"),
                Vector2D((float)(x * SPIDER_SPRITE_SIZE), (float)(y * SPIDER_SPRITE_SIZE)),
                SPIDER_SPRITE_SIZE, SPIDER_SPRITE_SIZE));
    mpAnimationManager->createAndAddAnimation(false, ss, "playerPunch"); ss.clear();

    // Die
    mpGraphicsBufferManager->createAndManageGraphicsBuffer("PlayerDeath", GAME_ASSET_PATH + "SpidermanSpriteDeath.png");
    for (unsigned int y = 0; y < SPIDER_DEATH_DOWN; y++)
        for (unsigned int x = 0; x < SPIDER_DEATH_ACROSS; x++)
            ss.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("PlayerDeath"),
                Vector2D((float)(x * SPIDER_SPRITE_SIZE), (float)(y * SPIDER_SPRITE_SIZE)),
                SPIDER_SPRITE_SIZE, SPIDER_SPRITE_SIZE));
    mpAnimationManager->createAndAddAnimation(false, ss, "playerDie"); ss.clear();

    mpPlayerManager->setAnimations(
        { *mpAnimationManager->getAnimation("playerIdle") },
        { *mpAnimationManager->getAnimation("playerWalk") },
        { *mpAnimationManager->getAnimation("playerShoot") },
        { *mpAnimationManager->getAnimation("playerBlast") },
        { *mpAnimationManager->getAnimation("playerBlock") },
        { *mpAnimationManager->getAnimation("playerPunch") },
        { *mpAnimationManager->getAnimation("playerDie") });

    mpEnemyManager = EnemyManager::createInstance(mpPlayerManager);
    mpEnemyManager->init();

    mpGraphicsBufferManager->createAndManageGraphicsBuffer("EnemyWalk", GAME_ASSET_PATH + "GOONVenomWalkingSpriteSheet.png");
    mpGraphicsBufferManager->createAndManageGraphicsBuffer("EnemyAttack", GAME_ASSET_PATH + "GOONVenomAttacking.png");
    mpGraphicsBufferManager->createAndManageGraphicsBuffer("EnemyDie", GAME_ASSET_PATH + "GOONVenomDeath.png");

    for (unsigned int y = 0; y < ENEMY_WALK_DOWN; y++)
        for (unsigned int x = 0; x < ENEMY_WALK_ACROSS; x++)
            ss.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("EnemyWalk"),
                Vector2D((float)(x * ENEMY_WALK_SIZE), (float)(y * ENEMY_WALK_SIZE)),
                ENEMY_WALK_SIZE, ENEMY_WALK_SIZE));
    mpAnimationManager->createAndAddAnimation(true, ss, "enemyWalk"); ss.clear();

    for (unsigned int y = 0; y < ENEMY_ATTACK_DOWN; y++)
        for (unsigned int x = 0; x < ENEMY_ATTACK_ACROSS; x++)
            ss.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("EnemyAttack"),
                Vector2D((float)(x * ENEMY_ATTACK_SIZE), (float)(y * ENEMY_ATTACK_SIZE)),
                ENEMY_ATTACK_SIZE, ENEMY_ATTACK_SIZE));
    mpAnimationManager->createAndAddAnimation(false, ss, "enemyAttack"); ss.clear();

    for (unsigned int y = 0; y < ENEMY_DIE_DOWN; y++)
        for (unsigned int x = 0; x < ENEMY_DIE_ACROSS; x++)
            ss.push_back(Sprite(mpGraphicsBufferManager->getGraphicsBuffer("EnemyDie"),
                Vector2D((float)(x * ENEMY_DIE_SIZE), (float)(y * ENEMY_DIE_SIZE)),
                ENEMY_DIE_SIZE, ENEMY_DIE_SIZE));
    mpAnimationManager->createAndAddAnimation(false, ss, "enemyDie"); ss.clear();

    mpEnemyManager->setWalkAnimations({ *mpAnimationManager->getAnimation("enemyWalk") });
    mpEnemyManager->setAttackAnimations({ *mpAnimationManager->getAnimation("enemyAttack") });
    mpEnemyManager->setDyingAnimations({ *mpAnimationManager->getAnimation("enemyDie") });

    // SpawnEvent is only for the legacy consuming-sound demo and not for spawning enemies
    EventSystem::getInstance()->fireEvent(SpawnEvent(Vector2D(0, 0)));


    mpMenuManager = new MenuManager();
    mpMenuManager->init(mpFont, WIDTH, HEIGHT);
    mpMenuManager->setFileManager(mpFileManager);

    mpHUD = HUD::createInstance(mpPlayerManager);
    mpHUD->init(mpFont);

    mpDeathScreen = DeathScreen::createInstance();
    mpDeathScreen->init(mpFont, WIDTH, HEIGHT);

    mpWinScreen = WinScreen::createInstance();
    mpWinScreen->init(mpFont, WIDTH, HEIGHT);

    mpLevelManager = LevelManager::createInstance();
    mpLevelManager->init(WORLD_WIDTH, WIDTH, false, TOTAL_LEVELS);

    mpSaveManager = SaveManager::getInstance();


    mpWebManager = WebManager::createInstance(mpEnemyManager);
    mpWebManager->init(mpFont);
    mpPlayerManager->setWebManager(mpWebManager);
    mpWebManager->setPlayerManager(mpPlayerManager);

    mpCamera = Camera::createInstance();
    mpCamera->init(WIDTH, HEIGHT, WORLD_WIDTH, HEIGHT);

    mpInputTranslator->setCamera(mpCamera);

    mpScrollBg = ScrollingBackground::createInstance();
    mpScrollBg->init(mpGraphicsBufferManager->getGraphicsBuffer("Background"),
        nullptr, nullptr, WIDTH, HEIGHT);


    // Boss frame size constants — would go in data file
    const unsigned int BOSS_IDLE_FRAME = 258;
    const unsigned int BOSS_WALK_FRAME = 257;
    const unsigned int BOSS_HURT_FRAME = 256;
    const unsigned int BOSS_DYING_FRAME = 257;
    const int BOSS_IDLE_COUNT = 7;
    const int BOSS_WALK_COUNT = 7;
    const int BOSS_HURT_COUNT = 2;
    const int BOSS_DEATH_COUNT = 4;

    mpBossManager = BossManager::createInstance(mpPlayerManager, mpCamera);
    mpBossManager->init(mpFont);
    mpBossManager->setGraphicsBufferManager(mpGraphicsBufferManager);

    std::string bp = GAME_ASSET_PATH + "01 [Default]BossVenom\\";
    auto makeFileList = [&](const std::string& stem, int count) {
        std::vector<std::string> files;
        for (int i = 0; i < count; i++)
        {
            std::string num = (i < 10) ? "0" + std::to_string(i) : std::to_string(i);
            files.push_back(bp + stem + "_" + num + ".png");
        }
        return files;
        };

    mpBossManager->loadBossAnimationsFromFiles(
        makeFileList("Selectscreenend", BOSS_IDLE_COUNT), BOSS_IDLE_FRAME, BOSS_IDLE_FRAME,
        makeFileList("Walk", BOSS_WALK_COUNT), BOSS_WALK_FRAME, BOSS_WALK_FRAME,
        makeFileList("Kd", BOSS_HURT_COUNT), BOSS_HURT_FRAME, BOSS_HURT_FRAME,
        makeFileList("Downbounce", BOSS_DEATH_COUNT), BOSS_DYING_FRAME, BOSS_DYING_FRAME);

    // Attack animations — order MUST match BossAttack enum (BIG_SWING=1 … TENTACLE=10)
    struct AttackInfo { std::string prefix; int frames; unsigned int w; unsigned int h; };
    AttackInfo attackInfos[] = {
        { "Attack1", 5, 257, 257 },  // BIG_SWING
        { "Attack3", 8, 257, 257 },  // TONGUE_ATTACK
        { "Attack4", 11, 513, 257 },  // LARGE_FORWARD
        { "Attackcharged", 7, 257, 257 },  // SIMPLE_JAB
        { "Attacksprinting", 12, 257, 257 },  // SHARK_CHARGE
        { "Parry", 10, 513, 257 },  // BIG_PUNCH
        { "Powerairgrabstomp", 4, 257, 257 },  // DOWNWARD_ATTACK
        { "Powerairgrabthrow",16, 257, 257 },  // DOWN_SPIKE
        { "Powerthrow2", 19, 257, 257 },  // CHARGE_FORWARD
        { "Special", 20, 513, 257 },  // TENTACLE_ATTACK
    };
    for (auto& info : attackInfos)
        mpBossManager->addAttackAnimationFromFiles(
            makeFileList(info.prefix, info.frames), info.w, info.h, false);

    mpWebManager->setBossManager(mpBossManager);

    // Each barrier is a trigger-less solid rectangle positioned at world edges
    mpPhysicsManager->createCollider("Sidewalk Barrier",
        WORLD_WIDTH - BARRIER_THICKNESS, BARRIER_THICKNESS,
        Vector2D(WORLD_WIDTH * 0.5f, (float)HEIGHT * 0.5f));
    mpPhysicsManager->createCollider("Left Barrier",
        BARRIER_THICKNESS, (float)(HEIGHT - BARRIER_THICKNESS),
        Vector2D(0.0f, (float)HEIGHT * 0.5f));
    mpPhysicsManager->createCollider("Bottom Barrier",
        WORLD_WIDTH - BARRIER_THICKNESS, BARRIER_THICKNESS,
        Vector2D(WORLD_WIDTH * 0.5f, (float)HEIGHT * 0.85f));

    mpAudioManager->playMusic(GameAudioManager::MusicTrack::MENU);
}


void Game::startNewGame()
{
    std::cout << "Game: starting new game\n";
    mCurrentScore = 0;
    mpHUD->resetScore();

    mpPlayerManager->resetPlayer(Vector2D((float)PLAYER_SPAWN_X, (float)PLAYER_SPAWN_Y));
    mpCamera->init(WIDTH, HEIGHT, WORLD_WIDTH, HEIGHT);
    mpLevelManager->init(WORLD_WIDTH, WIDTH, false, TOTAL_LEVELS);

    loadLevel(0);

    mpAudioManager->playMusic(GameAudioManager::MusicTrack::LEVEL);
}


void Game::loadSavedGame()
{
    if (!mpSaveManager->hasSaveFile(SAVE_FILE))
    {
        std::cout << "Game: no save file found — starting new game instead\n";
        startNewGame();
        return;
    }

    GameSaveData saveData;
    if (!mpSaveManager->loadGame(SAVE_FILE, saveData) || !saveData.valid)
    {
        std::cout << "Game: save file corrupt — starting new game instead\n";
        startNewGame();
        return;
    }

    std::cout << "Game: loading save (level " << saveData.level.currentLevel << ")\n";
    mCurrentScore = 0;
    mpHUD->resetScore();

    mpLevelManager->setCurrentLevel(saveData.level.currentLevel);
    loadLevel(saveData.level.currentLevel);

    if (saveData.boss.alive && mpBossManager)
    {
        mpBossManager->spawnBoss(
            Vector2D(saveData.boss.posX, saveData.boss.posY),
            saveData.boss.maxHp);
        mpBossManager->damageBoss(saveData.boss.maxHp - saveData.boss.hp);
    }


    auto& enemies = mpEnemyManager->getEnemies();
    for (int i = 0; i < (int)saveData.enemies.size() && i < (int)enemies.size(); i++)
    {
        enemies[i]->setPosition(Vector2D(saveData.enemies[i].posX,
            saveData.enemies[i].posY));
        enemies[i]->setHp(saveData.enemies[i].hp);
    }

    // Restore active web projectiles
    for (auto& wd : saveData.webs)
    {
        mpWebManager->getPool().acquire(Vector2D(wd.posX, wd.posY), Vector2D(wd.dirX, wd.dirY), wd.speed);
    }
    mpPlayerManager->setHp(saveData.player.hp);
    mpPlayerManager->resetPlayer(Vector2D(saveData.player.posX, saveData.player.posY));
    mpCamera->init(WIDTH, HEIGHT, WORLD_WIDTH, HEIGHT);

    mpAudioManager->playMusic(GameAudioManager::MusicTrack::LEVEL);
}


void Game::loadLevel(int level)
{
    mpEnemyManager->cleanup();
    bool hasBoss = (level == 1);
    mpLevelManager->setHasBoss(hasBoss);
    mpLevelManager->init(WORLD_WIDTH, WIDTH, hasBoss, TOTAL_LEVELS);



    if (level == 0)
    {
        mpScrollBg->setFarBackground(mpGraphicsBufferManager->getGraphicsBuffer("Background"));
        // Level 1 — lighter enemy layout, no boss
        mpEnemyManager->spawnEnemy(Vector2D(400.0f, (float)HEIGHT / 2.0f));
        mpEnemyManager->spawnEnemy(Vector2D(800.0f, (float)HEIGHT / 2.0f));
        mpEnemyManager->spawnStrongEnemy(Vector2D(2200.0f, (float)HEIGHT / 2.0f));
        mpEnemyManager->spawnEnemy(Vector2D(2600.0f, (float)HEIGHT / 2.0f));
    }
    else if (level == 1)
    {
        mpScrollBg->setFarBackground(mpGraphicsBufferManager->getGraphicsBuffer("Background2"));
        mpBossManager->spawnBoss(Vector2D(WORLD_WIDTH - BOSS_SPAWN_OFFSET, (float)(HEIGHT / 2)), BOSS_MAX_HP);
        // Level 2 — denser layout + boss at world end
       mpEnemyManager->spawnEnemy(Vector2D(1100.0f, (float)HEIGHT / 2.0f));
       mpEnemyManager->spawnStrongEnemy(Vector2D(1600.0f, (float)HEIGHT / 2.0f));
      
    }
}

void Game::cleanup()
{
    if (mpInputTranslator) { delete mpInputTranslator; mpInputTranslator = nullptr; }
    if (mpInputSystem) { mpInputSystem->cleanup(); delete mpInputSystem; mpInputSystem = nullptr; }
    if (mpBossManager) { mpBossManager->cleanup(); mpBossManager->destroyInstance(); mpBossManager = nullptr; }
    if (mpEnemyManager) { mpEnemyManager->cleanup(); mpEnemyManager->destroyInstance(); mpEnemyManager = nullptr; }
    if (mpWebManager) { mpWebManager->cleanup(); mpWebManager->destroyInstance(); mpWebManager = nullptr; }
    if (mpPlayerManager) { mpPlayerManager->cleanup(); mpPlayerManager->destroyInstance(); mpPlayerManager = nullptr; }
    if (mpWinScreen) { mpWinScreen->cleanup(); mpWinScreen->destroyInstance(); mpWinScreen = nullptr; }
    if (mpDeathScreen) { mpDeathScreen->cleanup(); mpDeathScreen->destroyInstance();  mpDeathScreen = nullptr; }
    if (mpHUD) { mpHUD->cleanup(); mpHUD->destroyInstance(); mpHUD = nullptr; }
    if (mpMenuManager) { mpMenuManager->cleanup();  delete mpMenuManager;  mpMenuManager = nullptr; }
    if (mpFont) { delete mpFont; mpFont = nullptr; }
    if (mpAudioManager) { mpAudioManager->cleanup(); mpAudioManager->destroyInstance(); mpAudioManager = nullptr; }
    if (mpLevelManager) { mpLevelManager->destroyInstance(); mpLevelManager = nullptr; }
    if (mpSaveManager) { mpSaveManager->destroyInstance();  mpSaveManager = nullptr; }
    if (mpScrollBg) { mpScrollBg->cleanup();  mpScrollBg->destroyInstance();   mpScrollBg = nullptr; }
    if (mpCamera) { mpCamera->destroyInstance();  mpCamera = nullptr; }
    if (mpAnimationManager) { mpAnimationManager->cleanup(); delete mpAnimationManager; mpAnimationManager = nullptr; }
    if (mpUnitManager) { mpUnitManager->cleanup(); delete mpUnitManager; mpUnitManager = nullptr; }
    if (mpGraphicsBufferManager) { mpGraphicsBufferManager->cleanup(); delete mpGraphicsBufferManager; mpGraphicsBufferManager = nullptr; }
    if (mpGraphicsSystem) { mpGraphicsSystem->cleanup(); delete mpGraphicsSystem; mpGraphicsSystem = nullptr; }
    if (mpPhysicsManager) { mpPhysicsManager->cleanup(); mpPhysicsManager->destroyInstance(); mpPhysicsManager = nullptr; }
    if (mpFileManager) { mpFileManager->destroyInstance(); mpFileManager = nullptr; }

    mAnimations.clear();
    EventSystem::getInstance()->removeListenerFromAllEvents(this);
}


bool Game::doLoop()
{
    PerformanceTracker tracker;
    Timer timer;

    while (mShouldContinue)
    {
        tracker.startTracking("loop");
        tracker.clearTracker("loop");
        timer.start();

        mpInputSystem->update();

        if (mpPlayerManager->getPlayer())
            mpCamera->update(mpPlayerManager->getPlayer()->getPosition(), TARGET_FPS);

        mpScrollBg->draw(mpCamera->getScrollX());

        // Audio updates every frame so music plays on menus too
        mpAudioManager->gameUpdate(TARGET_FPS);

        if (mpMenuManager->isGameRunning())
        {
            mpUnitManager->update(TARGET_FPS);
            mpUnitManager->draw(mpCamera->getOffset());
            mpPlayerManager->update(TARGET_FPS);
            mpPlayerManager->draw(mpCamera->getOffset());
            mpEnemyManager->update(TARGET_FPS);
            mpEnemyManager->draw(mpCamera->getOffset());
            mpBossManager->update(TARGET_FPS);
            mpBossManager->draw();
            mpWebManager->update(TARGET_FPS);
            mpWebManager->draw(mpCamera->getOffset());
            mpWebManager->drawHUD();
            mpHUD->update(TARGET_FPS);
            mpHUD->draw();
            mpBossManager->drawHUD();
            mpPhysicsManager->handleCollisions();
        }

        mpMenuManager->update(TARGET_FPS);
        mpMenuManager->draw();


        if (mpPlayerManager->isDead())
        {
            if (!mpDeathScreen->isActive())
            {
                mpDeathScreen->activate();
                mpAudioManager->playGameOver();
            }

            mpDeathScreen->update(TARGET_FPS);
            mpDeathScreen->draw();

            if (mpDeathScreen->wantsRestart())
            {
                // Restart from the beginning of the current level (no save load)
                mpPlayerManager->resetPlayer(Vector2D((float)PLAYER_SPAWN_X, (float)PLAYER_SPAWN_Y));
                mpCamera->init(WIDTH, HEIGHT, WORLD_WIDTH, HEIGHT);
                loadLevel(mpLevelManager->getCurrentLevel());
                mpDeathScreen->reset();
            }
            if (mpDeathScreen->wantsQuit())
            {
                mpPlayerManager->resetPlayer(Vector2D((float)PLAYER_SPAWN_X, (float)PLAYER_SPAWN_Y));
                mpCamera->init(WIDTH, HEIGHT, WORLD_WIDTH, HEIGHT);
                mpEnemyManager->cleanup();
                mpDeathScreen->reset();
                mpMenuManager->setState(MenuManager::MenuState::MAIN_MENU);
                mpAudioManager->playMusic(GameAudioManager::MusicTrack::MENU);
            }
        }


        if (mpMenuManager->isGameRunning() && !mpPlayerManager->isDead())
        {
            if (mpLevelManager->update(mpPlayerManager, mpBossManager))
            {
                bool isFinal = mpLevelManager->isFinalLevel();
                if (!mpWinScreen->isActive())
                    mpWinScreen->activate(
                        isFinal ? WinScreen::WinType::FINAL_WIN : WinScreen::WinType::LEVEL_WIN,
                        mpLevelManager->getCurrentLevel(), mCurrentScore);

                mpSaveManager->saveGame(SAVE_FILE, mpPlayerManager, mpEnemyManager,
                    mpWebManager, mpBossManager, mpLevelManager->getCurrentLevel());
            }
        }


        if (mpWinScreen && mpWinScreen->isActive())
        {
            mpWinScreen->update(TARGET_FPS);
            mpWinScreen->draw();

            if (mpWinScreen->wantsNextLevel())
            {
                mpLevelManager->advanceLevel();
                mpPlayerManager->resetPlayer(Vector2D((float)PLAYER_SPAWN_X, (float)PLAYER_SPAWN_Y));
                mpCamera->init(WIDTH, HEIGHT, WORLD_WIDTH, HEIGHT);
                loadLevel(mpLevelManager->getCurrentLevel());
                mpSaveManager->deleteSave(SAVE_FILE);
                mpWinScreen->reset();
            }
            if (mpWinScreen->wantsMainMenu())
            {
                mpPlayerManager->resetPlayer(Vector2D((float)PLAYER_SPAWN_X, (float)PLAYER_SPAWN_Y));
                mpCamera->init(WIDTH, HEIGHT, WORLD_WIDTH, HEIGHT);
                mpEnemyManager->cleanup();
                mpSaveManager->deleteSave(SAVE_FILE);
                mpWinScreen->reset();
                mpMenuManager->setState(MenuManager::MenuState::MAIN_MENU);
                mpAudioManager->playMusic(GameAudioManager::MusicTrack::MENU);
            }
        }

        mpGraphicsSystem->flip();
        timer.sleepUntilElapsed(TARGET_FPS);
        tracker.stopTracking("loop");
    }

    cleanup();
    return mRet;
}

void Game::handleEvent(const Event& theEvent)
{
    switch (theEvent.getType())
    {
    case QUIT_EVENT:
        mpSaveManager->saveGame(SAVE_FILE, mpPlayerManager, mpEnemyManager,
            mpWebManager, mpBossManager, mpLevelManager->getCurrentLevel());
        mShouldContinue = false;
        break;

    case MENU_PLAY_EVENT:
        // "Play New Game" — always starts fresh, ignores any existing save
        startNewGame();
        break;

    case MENU_LOAD_EVENT:
        // "Load Game" — restores from save file (falls back to new game if none)
        loadSavedGame();
        break;

    case SPAWN_EVENT:
        // Legacy demo: fire consuming sounds on spawn
        EventSystem::getInstance()->fireEvent(ConsumingEvent(380.0f, "consuming"));
        EventSystem::getInstance()->fireEvent(ConsumingEvent(320.0f, "Eat"));
        break;

    case DESTROY_EVENT:
    {
        const DestroyEvent& destroyEvent = static_cast<const DestroyEvent&>(theEvent);
        mpUnitManager->deleteUnit(mpUnitManager->getUnitAt(destroyEvent.getLocation()));
        break;
    }
    case DESTROY_ALL_EVENT:
        mpUnitManager->cleanup();
        break;

    case SWITCH_ANIMATION_EVENT:
        if (mpUnitManager->getRecentUnit() != nullptr)
            mpUnitManager->getRecentUnit()->switchAnimations();
        break;
    }
}












