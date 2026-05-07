#include "HUD.h"
#include "PlayerManager.h"
#include "GameEvent.h"
#include "EventSystem.h"
#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include "Font.h"
#include "LanguageManager.h"
#include "ScoreEvent.h"
#include "MenuEvent.h"
#include <SDL3/SDL.h>
#include <string>
#include <iostream>
#include "FileManager.h"

HUD* HUD::mpsInstance = nullptr;

HUD* HUD::createInstance(PlayerManager* pPlayerManager)
{
	if (mpsInstance == nullptr)
		mpsInstance = new HUD(pPlayerManager);
	return mpsInstance;
}

void HUD::destroyInstance()
{
	delete mpsInstance;
	mpsInstance = nullptr;
}

HUD::HUD(PlayerManager* pPlayerManager) : mpPlayerManager(pPlayerManager)
{
	EventSystem::getInstance()->addListener((EventType)SCORE_EVENT, this);
	EventSystem::getInstance()->addListener((EventType)MENU_DIFFICULTY_EVENT, this);
}

void HUD::init(Font* pFont)
{
	FileManager::getInstance()->readAndImportFile(DATA_FILE);
	mpFont = pFont;
}

void HUD::cleanup()
{
	EventSystem::getInstance()->removeListenerFromAllEvents(this);
	mpFont = nullptr;
}

void HUD::addScore(int amount)
{
	mScore += amount;
}

void HUD::update(double dtMs)
{
	mFpsTimer += (float)dtMs;
	mFpsCounter++;
	if (mFpsTimer >= 1000.0f)
	{
		mCurrentFPS = mFpsCounter;
		mFpsCounter = 0;
		mFpsTimer -= 1000.0f;
	}
}

void HUD::drawHealthBar()
{
	if (!mpPlayerManager) return;

	GraphicsBuffer* pBack = GraphicsSystem::getBackBuffer();
	if (!pBack) return;

	int maxHp = mpPlayerManager->getMaxHp();
	int curHp = mpPlayerManager->getHp();
	float fraction = (maxHp > 0) ? (float)curHp / (float)maxHp : 0.0f;
	if (fraction < 0.0f) fraction = 0.0f;

	if (fraction > 1.0f) fraction = 1.0f;

	SDL_Renderer* pRenderer = GraphicsBuffer::getRenderer();
	if (!pRenderer) return;

	SDL_SetRenderTarget(pRenderer, nullptr);

	SDL_FRect bgRect{ (float)BAR_X, (float)BAR_Y, (float)BAR_W, (float)BAR_H };
	SDL_SetRenderDrawColor(pRenderer, COLOR_BAR_BG.getR(), COLOR_BAR_BG.getG(), COLOR_BAR_BG.getB(), 255);
	SDL_RenderFillRect(pRenderer, &bgRect);

	float fillW = (float)(BAR_W - 2 * BAR_PADDING) * fraction;
	SDL_FRect fillRect{ (float)(BAR_X + BAR_PADDING), (float)(BAR_Y + BAR_PADDING), fillW, (float)(BAR_H - 2 * BAR_PADDING) };
	SDL_SetRenderDrawColor(pRenderer, COLOR_BAR_FILL.getR(), COLOR_BAR_FILL.getG(), COLOR_BAR_FILL.getB(), 255);
	SDL_RenderFillRect(pRenderer, &fillRect);

	SDL_SetRenderDrawColor(pRenderer, COLOR_BAR_FRAME.getR(), COLOR_BAR_FRAME.getG(), COLOR_BAR_FRAME.getB(), 255);
	SDL_RenderRect(pRenderer, &bgRect);

	if (mpFont)
	{
		std::string hpLabel = LS(StringID::HUD_HP);
		std::string hpText = hpLabel + ": " + std::to_string(curHp) + " / " + std::to_string(maxHp);
		GraphicsSystem::getBackBuffer()->writeText(Vector2D((float)(BAR_X + BAR_W + 8), (float)BAR_Y), *mpFont, COLOR_WHITE, hpText);
	}


}

void HUD::drawScore()
{
	if (!mpFont) return;

	GraphicsBuffer* pBack = GraphicsSystem::getBackBuffer();
	if (!pBack) return;

	std::string scoreLabel = LS(StringID::HUD_SCORE);
	std::string scoreText = scoreLabel + ": " + std::to_string(mScore);

	float x = (float)(SCREEN_W - 115);
	float y = 10.0f;

	pBack->writeText(Vector2D(x, y), *mpFont, COLOR_YELLOW, scoreText);
}

void HUD::drawFPS()
{
	if (!mpFont) return;

	GraphicsBuffer* pBack = GraphicsSystem::getBackBuffer();
	if (!pBack) return;

	std::string fpsLabel = LS(StringID::HUD_FPS);
	std::string fpsText = fpsLabel + ": " + std::to_string(mCurrentFPS);

	float x = (float)(SCREEN_W - 100);
	float y = (float)(SCREEN_H - 40);

	pBack->writeText(Vector2D(x, y), *mpFont, COLOR_GRAY, fpsText);
}

void HUD::draw()
{
	drawHealthBar();
	drawScore();
	drawFPS();
}

void HUD::handleEvent(const Event& theEvent)
{
	switch (theEvent.getType())
	{
	case SCORE_EVENT:
	{
		const ScoreEvent& scoreEvent = static_cast<const ScoreEvent&>(theEvent);
		addScore(scoreEvent.getAmount());
		break;
	}
	case MENU_DIFFICULTY_EVENT:
	{
		const MenuDifficultyEvent& e =
			static_cast<const MenuDifficultyEvent&>(theEvent);
		mDifficulty = e.getDifficulty();
		std::cout << "HUD: difficulty set to " << mDifficulty << "\n";
		break;
	}

	default:
		break;
	}
}

