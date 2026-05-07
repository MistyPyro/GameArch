#pragma once
#include "EventListener.h"
#include "Color.h"
#include "Vector2D.h"
#include <string>

class Font;
class FileManager;

class DeathScreen : public EventListener
{
	friend class FileManager;
public:
	static DeathScreen* createInstance();
	static DeathScreen* getInstance() { return mpsInstance; };
	static void destroyInstance();

	void init(Font* pFont, unsigned int screenW, unsigned int screenH);
	void cleanup();
	void update(double dtMs);
	void draw();
	bool isActive() const { return mActive; }
	bool wantsRestart() const { return mWantsRestart; }
	bool wantsQuit() const { return mWantsQuit; }
	void activate();
	void reset();
	void handleEvent(const Event& theEvent) {}

private:
	static DeathScreen* mpsInstance;
	DeathScreen() {};
	~DeathScreen() { cleanup(); }

	bool drawButton(const std::string& lable, float x, float y, float w, float h);
	Font* mpFont = nullptr;
	unsigned int mScreenW = 800;
	unsigned int mScreenH = 600;
	bool mActive = false;
	bool mWantsRestart = false;
	bool mWantsQuit = false;
	float mFadeTimer = 0.0f;
	float mMouseX = 0;
	float mMouseY = 0;
	bool mMousePressed = false;
	bool mMouseWasDown = false;

	inline void setFadeDuration(float duration) { FADE_DURATION = duration; };
	inline void setOverlayColor(Color color) { COL_OVERLAY = color; };
	inline void setPanelColor(Color color) { COL_PANEL = color; };
	inline void setButtonColor(Color color) { COL_BTN = color; };
	inline void setButtonHoverColor(Color color) { COL_BTN_HOV = color; };
	inline void setTextColor(Color color) { COL_TEXT = color; };
	inline void setTitleColor(Color color) { COL_TITLE = color; };

	const std::string DATA_FILE = "DeathScreenData.txt";

	// THESE ARE DATA DRIVEN - CHANGE FILE VALUES NOT THESE | values here are fallback if file cannot be read
	float FADE_DURATION = 1000.0f;
	Color COL_OVERLAY = Color(120, 0, 0, 200);
	Color COL_PANEL = Color(20, 0, 0, 230);
	Color COL_BTN = Color(80, 20, 20, 255);
	Color COL_BTN_HOV = Color(140, 40, 40, 255);
	Color COL_TEXT = Color(255, 255, 255, 255);
	Color COL_TITLE = Color(255, 60, 60, 255);
};