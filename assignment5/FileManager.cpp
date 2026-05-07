#include "FileManager.h"
#include <fstream>
#include <sstream>
#include "Game.h"
#include "EnemyManager.h"
#include <iostream>
#include "AudioManager.h"
#include "MenuManager.h"
#include "LanguageManager.h"
#include "WebManager.h"
#include "Color.h"
#include "LevelManager.h"
#include "BossManager.h"
#include "HUD.h"
#include "Camera.h"
#include "DeathScreen.h"
#include "GameAudioManager.h"
#include "PlayerManager.h"
#include "ScrollingBackground.h"
#include "WinScreen.h"

FileManager* FileManager::mpsInstance = nullptr;
using namespace std;

FileManager* FileManager::createInstance()
{
	if (mpsInstance == nullptr)
		mpsInstance = new FileManager;
	return mpsInstance;
}

void  FileManager::destroyInstance()
{
	delete mpsInstance;
	mpsInstance = nullptr;
}

bool FileManager::readAndImportFile(const string filename)
{
	ifstream fin(DATA_PATH + filename);

	if (!fin.is_open())
	{
		cout << "Unable to open file: " << filename << endl;
		return false;
	}

	string owner = "owner";
	getline(fin, owner);
	string temp = "temp";
	
	if (owner == OWNER_GAME)
	{
		Game* pGame = Game::getInstance();
		getline(fin, temp);
		pGame->setWidth(static_cast<unsigned int>(stoul(temp)));
		getline(fin, temp);
		pGame->setHeight(static_cast<unsigned int>(stoul(temp)));
		getline(fin, temp);
		pGame->setPlayerSpawnX(static_cast<unsigned int>(stoul(temp)));
		getline(fin, temp);
		pGame->setPlayerSpawnY(static_cast<unsigned int>(stoul(temp)));
		getline(fin, temp);
		pGame->setTargetFps(stod(temp));
		getline(fin, temp);
		pGame->setOutOfBoundsDist(stoi(temp));
		getline(fin, temp);
		pGame->setFontSize(stoi(temp));
		getline(fin, temp);
		pGame->setUnitSpeed(stof(temp));
		getline(fin, temp);
		pGame->setWorldWidth(stof(temp));
		getline(fin, temp);
		pGame->setTotalLevels(stoi(temp));
		getline(fin, temp);
		pGame->setBossSpawnOffset(stof(temp));
		getline(fin, temp);
		pGame->setBossMaxHP(stoi(temp));
		getline(fin, temp);
		pGame->setBarrierThickness(stof(temp));
		getline(fin, temp);
		pGame->setGameAssetPath(temp);
		pGame->setAudioGameAssetPath(temp);
		getline(fin, temp);
		pGame->setAssetPath(temp);
		getline(fin, temp);
		pGame->setAudioAssetPath(temp);
		getline(fin, temp);
		pGame->setSpidermanIdle(temp);
		getline(fin, temp);
		pGame->setSmurfFilename(temp);
		getline(fin, temp);
		pGame->setKenneyAssetPath(temp);
		getline(fin, temp);
		pGame->setKenneySmokePath(temp);
		getline(fin, temp);
		pGame->setFemalePersonFilename(temp);
		getline(fin, temp);
		pGame->setBGFilename(temp);
		getline(fin, temp);
		pGame->setFontFilename(temp);
	}
	else if (owner == OWNER_ENEMYMANAGER)
	{
		EnemyManager* pEnemyManager = EnemyManager::getInstance();

		getline(fin, temp);
		pEnemyManager->setEnemySpeed(stof(temp));
		getline(fin, temp);
		pEnemyManager->setEnemyHP(stoi(temp));
		getline(fin, temp);
		pEnemyManager->setEnemyAttackDamage(stoi(temp));
		getline(fin, temp);
		pEnemyManager->setEnemyAttackRange(stof(temp));
		getline(fin, temp);
		pEnemyManager->setEnemyAttackCooldown(stof(temp));
		getline(fin, temp);
		pEnemyManager->setStrongHPMultiplier(stof(temp));
		getline(fin, temp);
		pEnemyManager->setStrongDamageMultiplier(stof(temp));
		getline(fin, temp);
		pEnemyManager->setStrongSpeedMultiplier(stof(temp));
		getline(fin, temp);
		pEnemyManager->setStrongRangeMultiplier(stof(temp));
	}
	else if (owner == OWNER_PLAYER)
	{
		PlayerManager* pPlayerManager = PlayerManager::getInstance();

		getline(fin, temp);
		pPlayerManager->setShootDuration(stof(temp));
		getline(fin, temp);
		pPlayerManager->setBlastDuration(stof(temp));
		getline(fin, temp);
		pPlayerManager->setPunchingDuration(stof(temp));
		getline(fin, temp);
		pPlayerManager->setDyingDuration(stof(temp));
		getline(fin, temp);
		pPlayerManager->setPlayerColliderSize(stof(temp));
		getline(fin, temp);
		pPlayerManager->setPunchColliderSize(stof(temp));
		getline(fin, temp);
		pPlayerManager->setPunchHitboxOffset(stof(temp));
		getline(fin, temp);
		pPlayerManager->setWallPushForce(stof(temp));
		getline(fin, temp);
		pPlayerManager->setHPEasy(stoi(temp));
		getline(fin, temp);
		pPlayerManager->setHPNormal(stoi(temp));
		getline(fin, temp);
		pPlayerManager->setHPHard(stoi(temp));
		getline(fin, temp);
		pPlayerManager->setDifficultyEasy(stoi(temp));
		getline(fin, temp);
		pPlayerManager->setDifficultyNormal(stoi(temp));
		getline(fin, temp);
		pPlayerManager->setDifficultyHard(stoi(temp));
	}
	else if (owner == OWNER_SETTINGS)
	{
		getline(fin, temp);
		int lang = stoi(temp);
		LanguageManager::getInstance()->setLanguage((LanguageManager::Language)lang);

		getline(fin, temp);
		bool soundOn = (temp == "1");

		getline(fin, temp);
		int soundVol = stoi(temp);

		getline(fin, temp);
		bool musicOn = (temp == "1");

		getline(fin, temp);
		int musicVol = stoi(temp);

		cout << "Settings loaded: language=" << lang
			<< ", soundOn=" << soundOn << ", soundVol=" << soundVol
			<< ", musicOn=" << musicOn << ", musicVol=" << musicVol << endl;


	}
	else if (owner == OWNER_WEBMANAGER)
	{
		WebManager* pWebManager = WebManager::getInstance();

		getline(fin, temp);
		pWebManager->setWebCost(stof(temp));
		getline(fin, temp);
		pWebManager->setBlastCost(stof(temp));
		getline(fin, temp);
		pWebManager->setRegenDelay(stof(temp));
		getline(fin, temp);
		pWebManager->setWebSpeed(stof(temp));
		getline(fin, temp);
		pWebManager->setHitRadius(stof(temp));
		getline(fin, temp);
		pWebManager->setWebDamage(stoi(temp));
		getline(fin, temp);
		pWebManager->setBlastDamage(stoi(temp));
		getline(fin, temp);
		pWebManager->setBlastDuration(stof(temp));
		getline(fin, temp);
		pWebManager->setBlastMaxWidth(stof(temp));
		getline(fin, temp);
		pWebManager->setBlastHitRadius(stof(temp));
		getline(fin, temp);
		pWebManager->setBlockDamageReduction(stof(temp));
		getline(fin, temp);
		pWebManager->setBlockSpeedMultiplier(stof(temp));
		getline(fin, temp);
		pWebManager->setBarX(stof(temp));
		getline(fin, temp);
		pWebManager->setBarY(stof(temp));
		getline(fin, temp);
		pWebManager->setBarW(stof(temp));
		getline(fin, temp);
		pWebManager->setBarH(stof(temp));
		getline(fin, temp);
		pWebManager->setBarPadding(stof(temp));

		string r, g, b;
		
		fin.ignore(); //each color is separated by blank line for readability
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pWebManager->setBGColor(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pWebManager->setFillColor(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pWebManager->setBlastFillColor(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pWebManager->setFrameColor(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pWebManager->setTextColor(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pWebManager->setEmptyColor(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pWebManager->setBlockColor(Color(stoi(r), stoi(g), stoi(b)));
	}
	else if (owner == OWNER_LEVELMANAGER)
	{
		LevelManager* pLevelManager = LevelManager::getInstance();

		getline(fin, temp);
		pLevelManager->setEndZoneMargin(stof(temp));
	}
	else if (owner == OWNER_BOSSMANAGER)
	{
		BossManager* pBossManager = BossManager::getInstance();

		getline(fin, temp);
		pBossManager->setBarX(stof(temp));
		getline(fin, temp);
		pBossManager->setBarY(stof(temp));
		getline(fin, temp);
		pBossManager->setBarW(stof(temp));
		getline(fin, temp);
		pBossManager->setBarH(stof(temp));
	}
	else if (owner == OWNER_HUD)
	{
		HUD* pHUD = HUD::getInstance();

		getline(fin, temp);
		pHUD->setScreenWidth(static_cast<unsigned int>(stoul(temp)));
		getline(fin, temp);
		pHUD->setScreenHeight(static_cast<unsigned int>(stoul(temp)));
		getline(fin, temp);
		pHUD->setBarWidth(static_cast<unsigned int>(stoul(temp)));
		getline(fin, temp);
		pHUD->setBarHeight(static_cast<unsigned int>(stoul(temp)));
		getline(fin, temp);
		pHUD->setBarX(static_cast<unsigned int>(stoul(temp)));
		getline(fin, temp);
		pHUD->setBarY(static_cast<unsigned int>(stoul(temp)));
		getline(fin, temp);
		pHUD->setBarPadding(static_cast<unsigned int>(stoul(temp)));
		
		string r, g, b;

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pHUD->setBarBGColor(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pHUD->setBarFillColor(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pHUD->setBarFrameColor(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pHUD->setColorWhite(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pHUD->setColorYellow(Color(stoi(r), stoi(g), stoi(b)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		pHUD->setColorGray(Color(stoi(r), stoi(g), stoi(b)));
	}
	else if (owner == OWNER_CAMERA)
	{
		Camera* pCamera = Camera::getInstance();

		getline(fin, temp);
		pCamera->setLerpSpeed(stof(temp));
		getline(fin, temp);
		pCamera->setDeadZoneX(stof(temp));
	}
	else if (owner == OWNER_DEATHSCREEN)
	{
		DeathScreen* pDeathScreen = DeathScreen::getInstance();

		getline(fin, temp);
		pDeathScreen->setFadeDuration(stof(temp));

		string r, g, b, a;

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pDeathScreen->setOverlayColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pDeathScreen->setPanelColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pDeathScreen->setButtonColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pDeathScreen->setButtonHoverColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pDeathScreen->setTextColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pDeathScreen->setTitleColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));
	}
	else if (owner == OWNER_GAMEAUDIOMANAGER)
	{
		GameAudioManager* pGameAudioManager = GameAudioManager::getInstance();

		getline(fin, temp);
		pGameAudioManager->setWebShotSoundCooldown(stof(temp));
		getline(fin, temp);
		pGameAudioManager->setWebBlastSoundCooldown(stof(temp));
		getline(fin, temp);
		pGameAudioManager->setPlayerDamageSoundCooldown(stof(temp));
		getline(fin, temp);
		pGameAudioManager->setPlayerBlockSoundCooldown(stof(temp));
		getline(fin, temp);
		pGameAudioManager->setEnemyAttackSoundCooldown(stof(temp));
		getline(fin, temp);
		pGameAudioManager->setEnemyDeathSoundCooldown(stof(temp));
		getline(fin, temp);
		pGameAudioManager->setBossAttackSoundCooldown(stof(temp));
		getline(fin, temp);
		pGameAudioManager->setBossHurtSoundCooldown(stof(temp));
		getline(fin, temp);
		pGameAudioManager->setMenuSelectSoundCooldown(stof(temp));
	}
	else if (owner == OWNER_SCROLLINGBACKGROUND)
	{
		ScrollingBackground* pScrollingBackground = ScrollingBackground::getInstance();

		getline(fin, temp);
		pScrollingBackground->setFarParallax(stof(temp));
		getline(fin, temp);
		pScrollingBackground->setMidParallax(stof(temp));
		getline(fin, temp);
		pScrollingBackground->setNearParallax(stof(temp));
	}
	else if (owner == OWNER_WINSCREEN)
	{
		WinScreen* pWinScreen = WinScreen::getInstance();

		string r, g, b, a;

		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pWinScreen->setOverlayColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pWinScreen->setPanelColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pWinScreen->setButtonColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pWinScreen->setButtonHoverColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pWinScreen->setTextColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pWinScreen->setTitleColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));

		fin.ignore();
		getline(fin, r);
		getline(fin, g);
		getline(fin, b);
		getline(fin, a);
		pWinScreen->setGoldColor(Color(stoi(r), stoi(g), stoi(b), stoi(a)));
	}

	cout << "Imported data from: " << owner << endl;
	return true;
}

bool FileManager::writeToFile(const string filename, const string data)
{
	ofstream fout(filename);

	if (!fout.is_open())
	{
		cout << "Unable to open file: " << filename << endl;
		return false;
	}

	fout << data;

	return true;
}

bool FileManager::saveSettings(const string filename, bool soundOn, int soundVol, bool musicOn, int musicVol, int language)
{

	stringstream data;
	LanguageManager* pLM = LanguageManager::getInstance();
	data << OWNER_SETTINGS + "\n";
	data << language << "\n";
	data << (soundOn ? "1" : "0") << "\n";
	data << soundVol << "\n";
	data << (musicOn ? "1" : "0") << "\n";
	data << musicVol << "\n";

	
	if (writeToFile(filename, data.str()))
	{
		cout << "FileManager: settings saved to " << filename << "\n";
		return true;
	}

	cout << "failed to write to file: " << filename << endl;
	return false;

}

bool FileManager::loadSettings(const string filename)
{
	return readAndImportFile(filename);
}
