#pragma once
#include "Trackable.h"
#include <string>

using namespace std;

class FileManager : public Trackable
{
public:
	static FileManager* createInstance();
	static FileManager* getInstance() { return mpsInstance; };
	static void destroyInstance();

	bool readAndImportFile(const string filename);
	bool writeToFile(const string filename, const string data);

	bool saveSettings(const string filename, bool soundOn, int soundVol, bool musicOn, int musicVol, int language);
	bool loadSettings(const string filename);

private:
	static FileManager* mpsInstance;
	FileManager() {};
	~FileManager() {};

	const string DATA_PATH = "..\\Data\\";
	const string OWNER_GAME = "#game";
	const string OWNER_ENEMYMANAGER = "#enemymanager";
	const string OWNER_PLAYER = "#playermanager";
	const string OWNER_SETTINGS = "#settings";
	const string OWNER_WEBMANAGER = "#webmanager";
	const string OWNER_LEVELMANAGER = "#levelmanager";
	const string OWNER_BOSSMANAGER = "#bossmanager";
	const string OWNER_HUD = "#hud";
	const string OWNER_CAMERA = "#camera";
	const string OWNER_DEATHSCREEN = "#deathscreen";
	const string OWNER_GAMEAUDIOMANAGER = "#gameaudiomanager";
	const string OWNER_SCROLLINGBACKGROUND = "#scrollingbackground";
	const string OWNER_WINSCREEN = "#winscreen";
};