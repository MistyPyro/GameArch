#pragma once
#include <Trackable.h>
#include <SDL3/SDL.h>
#include <Vector2D.h>

class InputSystem : public Trackable
{
public:
    // Map our key constants to SDL3 scancodes
    enum KeyboardInputs
    {
        KEY_ESCAPE = SDL_SCANCODE_ESCAPE,
        KEY_SPACE = SDL_SCANCODE_SPACE,
        KEY_ENTER = SDL_SCANCODE_RETURN,
        KEY_F = SDL_SCANCODE_F,
        KEY_M = SDL_SCANCODE_M,
        KEY_S = SDL_SCANCODE_S,
        KEY_D = SDL_SCANCODE_D,
        KEY_1 = SDL_SCANCODE_1,
        KEY_2 = SDL_SCANCODE_2,
        KEY_3 = SDL_SCANCODE_3,
        KEY_W = SDL_SCANCODE_W,
        KEY_J = SDL_SCANCODE_J,
        KEY_K = SDL_SCANCODE_K,
        KEY_L = SDL_SCANCODE_L,
        KEY_A = SDL_SCANCODE_A,
        KEY_SEMICOLON = SDL_SCANCODE_SEMICOLON,
    };

    enum MouseInputs
    {
        MOUSE_LEFT = SDL_BUTTON_LEFT,
        MOUSE_RIGHT = SDL_BUTTON_RIGHT,
        MOUSE_MIDDLE = SDL_BUTTON_MIDDLE
    };

    InputSystem();
    ~InputSystem();

    bool init();
    void cleanup();
    void update();        

    bool isKeyPressed(KeyboardInputs key);
    bool isMousePressed(MouseInputs mouse);
    Vector2D getMousePosition();

private:
    void processSDLKeyEvent(const SDL_Event& event);
    void processSDLMouseMotionEvent(const SDL_Event& event);
    void processSDLMouseButtonEvent(const SDL_Event& event);

    Vector2D mMousePosition;
};