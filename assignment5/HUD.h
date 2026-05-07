#pragma once
#include "EventListener.h"
#include "Vector2D.h"
#include "Color.h"
#include <string>

class Font;
class GraphicsBuffer;
class PlayerManager;
class FileManager;

class HUD : public EventListener
{
	friend class FileManager;
public: 
	static HUD* createInstance(PlayerManager* pPlayerManager);
	static HUD* getInstance() { return mpsInstance; };
	static void destroyInstance();

	void init(Font* pFont);
	void cleanup();
	void update(double dtMs);
	void draw();
	void addScore(int amount);
	int getScore() const { return mScore; }
	void setDifficulty(int difficulty) { mDifficulty = difficulty; }
	int getDifficulty() const { return mDifficulty; }
	void handleEvent(const Event& theEvent);
	void resetScore() { mScore = 0; }

private:
	static HUD* mpsInstance;
	HUD(PlayerManager* pPlayerManager);
	~HUD() {
		std::cout << "~HUD\n";
		cleanup();
	}

	void drawHealthBar();
	void drawScore();
	void drawFPS();

	PlayerManager* mpPlayerManager = nullptr;
	Font* mpFont = nullptr;
	int mScore = 0;
	float mFpsTimer = 0.0f;
	int mFpsCounter = 0;
	int mCurrentFPS = 0;
	int mDifficulty = 1;
	
	inline void setScreenWidth(unsigned int width) { SCREEN_W = width; };
	inline void setScreenHeight(unsigned int height) { SCREEN_H = height; };
	inline void setBarWidth(unsigned int width) { BAR_W = width; };
	inline void setBarHeight(unsigned int height) { BAR_H = height; };
	inline void setBarX(unsigned int x) { BAR_X = x; };
	inline void setBarY(unsigned int y) { BAR_Y = y; };
	inline void setBarPadding(unsigned int padding) { BAR_PADDING = padding; };
	inline void setBarBGColor(Color color) { COLOR_BAR_BG = color; };
	inline void setBarFillColor(Color color) { COLOR_BAR_FILL = color; };
	inline void setBarFrameColor(Color color) { COLOR_BAR_FRAME = color; };
	inline void setColorWhite(Color color) { COLOR_WHITE = color; };
	inline void setColorYellow(Color color) { COLOR_YELLOW = color; };
	inline void setColorGray(Color color) { COLOR_GRAY = color; };

	const std::string DATA_FILE = "HUDData.txt";

	// THESE ARE DATA DRIVEN - CHANGE FILE VALUES NOT THESE | values here are fallback if file cannot be read
	unsigned int SCREEN_W = 800;
	unsigned int SCREEN_H = 600;
	unsigned int BAR_W = 200;
	unsigned int BAR_H = 20;
	unsigned int BAR_X = 10;
	unsigned int BAR_Y = 10;
	unsigned int BAR_PADDING = 2;
	Color COLOR_BAR_BG = Color(80, 0, 0);
	Color COLOR_BAR_FILL = Color(220, 30, 30);
	Color COLOR_BAR_FRAME = Color(255, 255, 255);
	Color COLOR_WHITE = Color(255, 255, 255);
	Color COLOR_YELLOW = Color(255, 220, 0);
	Color COLOR_GRAY = Color(180, 180, 180);
};