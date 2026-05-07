#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_ttf.h>
#include <iostream>
#include <cassert>

using namespace std;

GraphicsBuffer* GraphicsSystem::mpBackBuffer = nullptr;

GraphicsSystem::GraphicsSystem()
{
    mWidth = 0;
    mHeight = 0;
}

GraphicsSystem::~GraphicsSystem()
{
    cleanup();
}

bool GraphicsSystem::init(unsigned int width, unsigned int height)
{
    if (mWindow)
        cleanup();

    // Initialize SDL3
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    // Initialize SDL_ttf
    if (!TTF_Init())
    {
        cerr << "TTF_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    mWidth = width;
    mHeight = height;

    mWindow = SDL_CreateWindow("Game", (int)width, (int)height, 0);
    if (!mWindow)
    {
        cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, nullptr);
    if (!mRenderer)
    {
        cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
        return false;
    }

    // Share renderer with GraphicsBuffer so all buffers can use it
    GraphicsBuffer::sRenderer = mRenderer;

    // The "back buffer" wraps the default render target (screen)
    mpBackBuffer = new GraphicsBuffer(mRenderer, width, height);

    return true;
}

void GraphicsSystem::cleanup()
{
    if (mpBackBuffer)
    {
        delete mpBackBuffer;
        mpBackBuffer = nullptr;
    }
    if (mRenderer)
    {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
        GraphicsBuffer::sRenderer = nullptr;
    }
    if (mWindow)
    {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    TTF_Quit();
    SDL_Quit();
}

void GraphicsSystem::flip()
{
    SDL_RenderPresent(mRenderer);
}