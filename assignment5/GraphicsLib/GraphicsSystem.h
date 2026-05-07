#pragma once
#include <Trackable.h>
#include <SDL3/SDL.h>
class GraphicsBuffer;
class Sprite;

class GraphicsSystem : public Trackable
{
public:
    GraphicsSystem();
    ~GraphicsSystem();

    bool init(unsigned int width, unsigned int height);
    void cleanup();

    unsigned int getDisplayWidth()  const { return mWidth; }
    unsigned int getDisplayHeight() const { return mHeight; }

    static GraphicsBuffer* getBackBuffer() { return mpBackBuffer; }

    void flip(); // present the rendered frame

private:
    SDL_Window* mWindow = nullptr;
    SDL_Renderer* mRenderer = nullptr;

    static GraphicsBuffer* mpBackBuffer;

    unsigned int mWidth = 0;
    unsigned int mHeight = 0;
};