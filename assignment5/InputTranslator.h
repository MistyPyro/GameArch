#pragma once
#include "Trackable.h"
#include "EventListener.h"
#include "Vector2D.h"
#include "Camera.h"

class Camera;
class PlayerManager;
class InputTranslator : public EventListener
{
public:
	InputTranslator();
	~InputTranslator();

	void handleEvent(const Event& theEvent);

	void setPlayerManager(PlayerManager* pPlayerManager) { mpPlayerManager = pPlayerManager; }

	void setCamera(Camera* p) { mpCamera = p; }

private:
	Vector2D mLastMousePos;
	PlayerManager* mpPlayerManager = nullptr;
	Camera* mpCamera = nullptr;
	const Vector2D UP = Vector2D(0, -1);
	const Vector2D DOWN = Vector2D(0, 1);
	const Vector2D LEFT = Vector2D(-1, 0);
	const Vector2D RIGHT = Vector2D(1, 0);

};