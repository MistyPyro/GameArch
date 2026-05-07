#include "Camera.h"
#include <cmath>
#include "FileManager.h"

Camera* Camera::mpsInstance = nullptr;

Camera* Camera::createInstance()
{
	if (mpsInstance == nullptr)
		mpsInstance = new Camera;
	return mpsInstance;
}

void Camera::destroyInstance()
{
	delete mpsInstance;
	mpsInstance = nullptr;
}

void Camera::init(unsigned int screenW, unsigned int screenH, float worldW, float worldH)
{
	FileManager::getInstance()->readAndImportFile(DATA_FILE);
	mScreenW = screenW;
	mScreenH = screenH;
	mWorldW = worldW;
	mWorldH = worldH;
	mOffset = Vector2D(0, 0);
}

void Camera::update(Vector2D playerPos, double dt)
{
	float targetX = playerPos.getX() - (float)mScreenW / 2.0f;
	if (targetX < 0) targetX = 0;

	if (targetX > mWorldW - mScreenW) targetX = mWorldW - mScreenW;

	float dtSec = (float)dt / 1000.0f;
	float currentX = mOffset.getX();
	float newX = currentX + (targetX - currentX) * LERP_SPEED * dtSec;
	mOffset = Vector2D(newX, 0.0f);
}