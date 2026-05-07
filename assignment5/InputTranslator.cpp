#include "InputTranslator.h"
#include "EventSystem.h"
#include "GameEvent.h"
#include "InputSystem.h"
#include "MouseEvent.h"
#include "SpawnEvent.h"
#include "DestroyEvent.h"
#include "MoveEvent.h"
#include "WebEvent.h"
#include "PunchEvent.h"
#include "PlayerManager.h"

InputTranslator::InputTranslator()
{
	EventSystem* pEventSystem = EventSystem::getInstance();
	pEventSystem->addListener((EventType)KEY_DOWN, this);
	pEventSystem->addListener((EventType)KEY_UP, this);
	pEventSystem->addListener((EventType)MOUSE_ACTION_EVENT, this);
}

InputTranslator::~InputTranslator()
{
	EventSystem::getInstance()->removeListenerFromAllEvents(this);
}

void InputTranslator::handleEvent(const Event& theEvent)
{
	EventSystem* pEventSystem = EventSystem::getInstance();
	const InputEvent& inputEvent = static_cast<const InputEvent&>(theEvent);

	switch (theEvent.getType())
	{
	case KEY_DOWN:
		switch (inputEvent.getKeyCode())
		{
		case InputSystem::KEY_ESCAPE:
			pEventSystem->fireEvent(GameEvent(MENU_PAUSE_EVENT));
			break;
		case InputSystem::KEY_W:
			pEventSystem->fireEvent(MoveEvent(UP, false));
			break;
		case InputSystem::KEY_A:
			pEventSystem->fireEvent(MoveEvent(LEFT, false));
			break;
		case InputSystem::KEY_S:
			pEventSystem->fireEvent(MoveEvent(DOWN, false));
			break;
		case InputSystem::KEY_D:
			pEventSystem->fireEvent(MoveEvent(RIGHT, false));
			break;
		case InputSystem::KEY_J:
			pEventSystem->fireEvent(PunchEvent(true));
			break;
		case InputSystem::KEY_L:
		{
			Vector2D playerPos = mpPlayerManager
				? mpPlayerManager->getPosition()
				: Vector2D(400, 300);
			pEventSystem->fireEvent(WebBlastEvent(playerPos, mLastMousePos));
			break;
		}
		case InputSystem::KEY_SEMICOLON:
			pEventSystem->fireEvent(BlockEvent(true));
			break;
		}
		break;
	case KEY_UP:
		switch (inputEvent.getKeyCode())
		{
		case InputSystem::KEY_J:
			pEventSystem->fireEvent(PunchEvent(false));
			break;
		case InputSystem::KEY_SEMICOLON:
			pEventSystem->fireEvent(BlockEvent(false));
			break;
		case InputSystem::KEY_W:
			pEventSystem->fireEvent(MoveEvent(UP, true));
			break;
		case InputSystem::KEY_A:
			pEventSystem->fireEvent(MoveEvent(LEFT, true));
			break;
		case InputSystem::KEY_S:
			pEventSystem->fireEvent(MoveEvent(DOWN, true));
			break;
		case InputSystem::KEY_D:
			pEventSystem->fireEvent(MoveEvent(RIGHT, true));
			break;
		}
		break;
	case MOUSE_ACTION_EVENT:
	{
		const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(theEvent);
		mLastMousePos = mouseEvent.getLocation();

		Vector2D cameraOffset = mpCamera ? mpCamera->getOffset() : Vector2D(0, 0);
		Vector2D worldMousePos = mouseEvent.getLocation() + cameraOffset;
		mLastMousePos = worldMousePos;

		Vector2D playerPos = mpPlayerManager ? mpPlayerManager->getPosition() : Vector2D(400, 300);

		if (mouseEvent.getKeyCode() == InputSystem::MOUSE_RIGHT && mouseEvent.isPressed())
			pEventSystem->fireEvent(WebFireEvent(playerPos, worldMousePos));
		break;
	}
	}
}
