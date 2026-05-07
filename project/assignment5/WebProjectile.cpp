#include "WebProjectile.h"
#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include <SDL3/SDL.h>
#include <cmath>

void WebProjectile::update(double dt)
{
    if (!mActive) return;

    mLifetimeMs += (float)dt;
    if (mLifetimeMs >= MAX_LIFETIME_MS)
    {
        mActive = false;
        return;
    }

    float dtSec = (float)dt / 1000.0f;
    mPosition = Vector2D(
        mPosition.getX() + mDirection.getX() * mSpeed * dtSec,
        mPosition.getY() + mDirection.getY() * mSpeed * dtSec);
}

void WebProjectile::draw(Vector2D cameraOffset)
{
    if (!mActive) return;

    SDL_Renderer* pR = GraphicsBuffer::getRenderer();
    if (!pR) return;
    SDL_SetRenderTarget(pR, nullptr);

    const int RADIUS = 5;
    float cx = mPosition.getX() - cameraOffset.getX();
    float cy = mPosition.getY() - cameraOffset.getY();

    // Dot
    SDL_SetRenderDrawColor(pR, 220, 220, 225, 255);
    SDL_FRect dot{ cx - RADIUS, cy - RADIUS, (float)(RADIUS * 2), (float)(RADIUS * 2) };
    SDL_RenderFillRect(pR, &dot);

    // Tail
    SDL_SetRenderDrawColor(pR, 200, 200, 240, 180);
    float tailX = cx - mDirection.getX() * 18.0f;
    float tailY = cy - mDirection.getY() * 18.0f;
    SDL_RenderLine(pR, (int)cx, (int)cy, (int)tailX, (int)tailY);
}

