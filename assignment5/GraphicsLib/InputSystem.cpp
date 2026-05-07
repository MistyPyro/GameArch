#include "InputSystem.h"
#include "EventSystem.h"
#include "InputEvent.h"
#include "MouseEvent.h"
#include "../GameEvent.h"
#include <iostream>

InputSystem::InputSystem()
    : mMousePosition(0, 0)
{
}

InputSystem::~InputSystem()
{
    cleanup();
}

bool InputSystem::init()
{
    return true;
}

void InputSystem::cleanup()
{
    
}


void InputSystem::processSDLKeyEvent(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        // event.key.scancode maps directly to our KeyboardInputs enum values
        EventSystem::getInstance()->fireEvent(InputEvent(KEY_DOWN, (int)event.key.scancode));
       
    }
    else if (event.type == SDL_EVENT_KEY_UP)
    {
        EventSystem::getInstance()->fireEvent(InputEvent(KEY_UP, (int)event.key.scancode));
        
    }
}

void InputSystem::processSDLMouseMotionEvent(const SDL_Event& event)
{
    mMousePosition = Vector2D((float)event.motion.x, (float)event.motion.y);
   
}

void InputSystem::processSDLMouseButtonEvent(const SDL_Event& event)
{
    Vector2D loc((float)event.button.x, (float)event.button.y);
    mMousePosition = loc;

    bool pressed = (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
    EventSystem::getInstance()->fireEvent(MouseEvent(loc, (int)event.button.button, pressed));

}


void InputSystem::update()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            // Translate window-close into our QUIT_EVENT
            EventSystem::getInstance()->fireEvent(GameEvent(QUIT_EVENT));
            break;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if (!event.key.repeat)
                processSDLKeyEvent(event);
            break;

        case SDL_EVENT_MOUSE_MOTION:
            processSDLMouseMotionEvent(event);
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            processSDLMouseButtonEvent(event);
            break;

        default:
            break;
        }
    }
}

bool InputSystem::isKeyPressed(KeyboardInputs key)
{
    int numKeys = 0;
    const bool* pState = SDL_GetKeyboardState(&numKeys);
    if ((int)key < numKeys)
        return pState[(int)key];
    return false;
}

bool InputSystem::isMousePressed(MouseInputs mouse)
{
    float mx, my;
    SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mx, &my);
    return (buttons & SDL_BUTTON_MASK((int)mouse)) != 0;
}

Vector2D InputSystem::getMousePosition()
{
    float mx, my;
    SDL_GetMouseState(&mx, &my);
    return Vector2D(mx, my);
}