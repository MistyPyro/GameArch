#pragma once
#include "EventListener.h"
#include "Color.h"
#include "Vector2D.h"
#include <string>

class Font;

class WinScreen : public EventListener
{
	friend class FileManager;
public:
	enum class WinType
	{
		LEVEL_WIN,
		FINAL_WIN	
	};

	static WinScreen* createInstance();
	static WinScreen* getInstance() { return mpsInstance; };
	static void destroyInstance();
	
	void init(Font* pFont, unsigned int screenW, unsigned int screenH);
	void cleanup();
	void activate(WinType type, int levelJustCompleted, int score);
	void reset();
	void update(double dtMs);
	void draw();
	bool isActive() const { return mActive; }
	bool wantsNextLevel() const { return mWantsNextLevel; }
	bool wantsMainMenu() const { return mWantsMainMenu; }
	void handleEvent(const Event& theEvent) {}

private:
	static WinScreen* mpsInstance;
	WinScreen() {};
	~WinScreen() { cleanup(); }

	bool drawButton(const std::string& label, float x, float y, float w, float h);
	Font* mpFont = nullptr;
	unsigned int mScreenW = 800;
	unsigned int mScreenH = 600;
	bool mActive = false;
	WinType mWinType = WinType::LEVEL_WIN;
	int mLevel = 0;
	int mScore = 0;
	bool mWantsNextLevel = false;
	bool mWantsMainMenu = false;
	float mFadeTimer = 0.0f;
	const float FADE_DURATION = 800.0f;
	float mMouseX = 0;
	float mMouseY = 0;
	bool mMousePressed = false;
	bool mMouseWasDown = false;

	inline void setOverlayColor(Color color) { COL_OVERLAY = color; };
	inline void setPanelColor(Color color) { COL_PANEL = color; };
	inline void setButtonColor(Color color) { COL_BTN = color; };
	inline void setButtonHoverColor(Color color) { COL_BTN_HOV = color; };
	inline void setTextColor(Color color) { COL_TEXT = color; };
	inline void setTitleColor(Color color) { COL_TITLE = color; };
	inline void setGoldColor(Color color) { COL_GOLD = color; };

	const std::string DATA_FILE = "WinScreenData.txt";

	// THESE ARE DATA DRIVEN - CHANGE FILE VALUES NOT THESE | values here are fallback if file cannot be read
	Color COL_OVERLAY = Color(0, 80, 0, 200);
	Color COL_PANEL = Color(10, 30, 10, 230);
	Color COL_BTN = Color(20, 80, 20, 255);
	Color COL_BTN_HOV = Color(40, 140, 40, 255);
	Color COL_TEXT = Color(255, 255, 255, 255);
	Color COL_TITLE = Color(100, 255, 100, 255);
	Color COL_GOLD = Color(255, 215, 0, 255);
};