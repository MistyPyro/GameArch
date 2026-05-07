#pragma once
#include <Trackable.h>
#include <string>
#include <vector>
#include "Color.h"
#include "EventListener.h"
#include "Animation.h"

using namespace std;

class GraphicsSystem;
class InputSystem;
class Sprite;
class GraphicsBufferManager;
class UnitManager;
class Font;
class InputTranslator;
class AnimationManager;
class AudioManager;
class PlayerManager;
class EnemyManager;
class HUD;
class MenuManager;
class WebManager;
class Camera;
class ScrollingBackground;
class BossManager;
class FileManager;
class DeathScreen;
class PhysicsManager;
class WinScreen;
class LevelManager;
class SaveManager;
class GameAudioManager;

class Game : public EventListener
{
	friend class FileManager;
public:
	static Game* createInstance();
	static Game* getInstance() { return mpsInstance; };
	static void destroyInstance();
	void init();
	void cleanup();
	bool doLoop();
	void handleEvent(const Event& theEvent);
	void startNewGame();
	void loadSavedGame();

private:
	static Game* mpsInstance;
	Game() {};
	~Game();

	GraphicsSystem* mpGraphicsSystem = nullptr;
	InputSystem* mpInputSystem = nullptr;
	GraphicsBufferManager* mpGraphicsBufferManager = nullptr;
	UnitManager* mpUnitManager = nullptr;
	PlayerManager* mpPlayerManager = nullptr;
	AnimationManager* mpAnimationManager = nullptr;
	GameAudioManager* mpAudioManager = nullptr;
	InputTranslator* mpInputTranslator = nullptr;
	EnemyManager* mpEnemyManager = nullptr;
	HUD* mpHUD = nullptr;
	MenuManager* mpMenuManager = nullptr;
	WebManager* mpWebManager = nullptr;
	FileManager* mpFileManager = nullptr;
	Camera* mpCamera = nullptr;
	ScrollingBackground* mpScrollBg = nullptr;
	BossManager* mpBossManager = nullptr;
	DeathScreen* mpDeathScreen = nullptr;
	PhysicsManager* mpPhysicsManager = nullptr;
	WinScreen* mpWinScreen = nullptr;
	LevelManager* mpLevelManager = nullptr;
	SaveManager* mpSaveManager = nullptr;
	int mCurrentScore = 0;


	inline void setWidth(unsigned int width) { WIDTH = width; };
	inline void setHeight(unsigned int height) { HEIGHT = height; };
	inline void setPlayerSpawnX(unsigned int x) { PLAYER_SPAWN_X = x; };
	inline void setPlayerSpawnY(unsigned int y) { PLAYER_SPAWN_Y = y; };
	inline void setTargetFps(double targetFps) { TARGET_FPS = targetFps; };
	inline void setOutOfBoundsDist(int ooBDist) { OUT_OF_BOUNDS_DIST = ooBDist; };
	inline void setFontSize(int size) { FONT_SIZE = size; };
	inline void setUnitSpeed(float speed) { UNIT_SPEED = speed; };
	inline void setGameAssetPath(string path) { GAME_ASSET_PATH = path; };
	inline void setAudioGameAssetPath(string path) { AUDIO_GAME_ASSET_PATH = path; };
	inline void setAssetPath(string path) { ASSET_PATH = path; };
	inline void setAudioAssetPath(string path) { AUDIO_ASSET_PATH = path; };
	inline void setSpidermanIdle(string path) { SPIDERMAN_IDLE_NAME = path; };
	inline void setSmurfFilename(string path) { SMURF_FILENAME = path; };
	inline void setKenneyAssetPath(string path) { KENNEY_ASSET_PATH = path; };
	inline void setKenneySmokePath(string path) { KENNEY_SMOKE_PATH = path; };
	inline void setFemalePersonFilename(string name) { FPERSON_FILENAME = name; };
	inline void setBGFilename(string name) { BG_FILENAME = name; };
	inline void setFontFilename(string name) { FONT_FILENAME = name; };
	inline void setWorldWidth(float width) { WORLD_WIDTH = width; };
	inline void setTotalLevels(int numLevels) { TOTAL_LEVELS = numLevels; };
	inline void setBossSpawnOffset(float offset) { BOSS_SPAWN_OFFSET = offset; };
	inline void setBossMaxHP(int hp) { BOSS_MAX_HP = hp; };
	inline void setBarrierThickness(float thickness) { BARRIER_THICKNESS = thickness; };

	bool mShouldContinue = true;
	bool mRet = false;

	string SAVE_FILE = "SaveGame.txt";

	void loadLevel(int level);

	Font* mpFont = nullptr;
	std::vector<Animation> mAnimations;

	//https://stackoverflow.com/questions/31531273/initializing-static-constexpr-variables-and-classes-inside-a-struct
	//https://stackoverflow.com/questions/31531273/initializing-static-constexpr-variables-and-classes-inside-a-struct#:~:text=2%20Answers&text=A%20static%20constexpr%20member%20has,the%20next%20compiler%20version%20might.&text=Sign%20up%20to%20request%20clarification%20or%20add%20additional%20context%20in%20comments.
	//https://www.youtube.com/watch?v=QVHwOOrSh3w&t=50s
	//https://stackoverflow.com/questions/13865842/does-static-constexpr-variable-inside-a-function-make-sense#:~:text=Comments,-Add%20a%20comment&text=In%20addition%20to%20given%20answer,const%20in%20combination%20with%20static%20.&text=As%20you%20can%20see%20yourself,initialization%20is%20performed%20only%20once.
	//https://stackoverflow.com/questions/16287776/static-constexpr-variable-vs-function
	//https://cplusplus.com/forum/general/277656/
	//https://en.wikipedia.org/wiki/Constexpr

	const string DATA_FILE = "GameData.txt";

	// THESE ARE DATA DRIVEN - CHANGE FILE VALUES NOT THESE | values here are fallback if file cannot be read
	unsigned int WIDTH = 800;
	unsigned int HEIGHT = 600;
	unsigned int PLAYER_SPAWN_X = 300;
	unsigned int PLAYER_SPAWN_Y = 475;
	double TARGET_FPS = 16.7;
	int OUT_OF_BOUNDS_DIST = 130;
	int FONT_SIZE = 18;
	float UNIT_SPEED = 0.487f;
	float WORLD_WIDTH = 3200.0f;
	int TOTAL_LEVELS = 3;
	float BOSS_SPAWN_OFFSET = 400.0f;
	int BOSS_MAX_HP = 30;
	float BARRIER_THICKNESS = 50.0f;

	string GAME_ASSET_PATH = "..\\Assets\\";
	string AUDIO_GAME_ASSET_PATH = "..\\Assets\\";
	string ASSET_PATH = "..\\..\\shared\\assets\\";
	string AUDIO_ASSET_PATH = "..\\..\\shared\\assets\\minetrap\\";
	string SPIDERMAN_IDLE_NAME = "spidermanspritesheetEating";
	string SMURF_FILENAME = "smurf_sprites.png";
	string KENNEY_ASSET_PATH = "kenney_toon-characters\\Female person\\PNG\\Poses\\";
	string KENNEY_SMOKE_PATH = "kenney_smoke-particles\\PNG\\Explosion\\";
	string FPERSON_FILENAME = "character_femalePerson_walk";
	string BG_FILENAME = "backgroundStreetBig.png";
	string FONT_FILENAME = "cour.ttf";
};