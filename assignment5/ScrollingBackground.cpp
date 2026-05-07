#include "ScrollingBackground.h"
#include "GraphicsBuffer.h"
#include "GraphicsSystem.h"
#include "Vector2D.h"
#include <SDL3/SDL.h>
#include <cmath>
#include "FileManager.h"

ScrollingBackground* ScrollingBackground::mpsInstance = nullptr;

ScrollingBackground* ScrollingBackground::createInstance()
{
	if (mpsInstance == nullptr)
		mpsInstance = new ScrollingBackground;
	return mpsInstance;
}

void ScrollingBackground::destroyInstance()
{
	delete mpsInstance;
	mpsInstance = nullptr;
}

void ScrollingBackground::init(GraphicsBuffer* pFarBg, GraphicsBuffer* pMidBg, GraphicsBuffer* pNearBg, unsigned int screenW, unsigned int screenH)
{
	FileManager::getInstance()->readAndImportFile(DATA_FILE);
	mpFarBg = pFarBg;
	mpMidBg = pMidBg;
	mpNearBg = pNearBg;
	mScreenH = screenH;
	mScreenW = screenW;
}

void ScrollingBackground::drawLayer(GraphicsBuffer* pBuf, float scrollX, float parallaxFactor, unsigned int screenW, unsigned int screenH)
{
	if (!pBuf) return;

	float layerScrollX = scrollX * parallaxFactor;
	unsigned int texW = pBuf->getWidth();
	unsigned int texH = pBuf->getHeight();
	float offsetX = std::fmod(layerScrollX, (float)texW);
	if (offsetX < 0) offsetX += texW;

	float scaleY = (float)screenH / (float)texH;
	float drawW = (float)texW * scaleY;
	float drawH = (float)screenH;
	GraphicsBuffer* pBack = GraphicsSystem::getBackBuffer();
	if (!pBack) return;
	
	float startX = -offsetX * scaleY;
	for (float x = startX; x < (float)screenW; x += drawW)
	{
		pBack->draw(Vector2D(x, 0.0f), *pBuf, scaleY);
	}
}

void ScrollingBackground::draw(float cameraScrollX)
{
	drawLayer(mpFarBg, cameraScrollX, FAR_PARALLAX, mScreenW, mScreenH);
	drawLayer(mpMidBg, cameraScrollX, MID_PARALLAX, mScreenW, mScreenH);
	drawLayer(mpNearBg, cameraScrollX, NEAR_PARALLAX, mScreenW, mScreenH);
}

void ScrollingBackground::cleanup()
{
	mpFarBg = mpMidBg = mpNearBg = nullptr;
}