#include "GraphicsBuffer.h"
#include "GraphicsSystem.h"
#include "Sprite.h"
#include "Vector2D.h"
#include <SDL3/SDL_ttf.h>
#include <cassert>
#include <iostream>

SDL_Renderer* GraphicsBuffer::sRenderer = nullptr;

// Redirects SDL rendering to a given texture, restores the previous target
// when the guard goes out of scope.
struct RenderTargetGuard
{
    SDL_Renderer* r;
    SDL_Texture* prev;
    RenderTargetGuard(SDL_Renderer* renderer, SDL_Texture* target)
        : r(renderer)
        , prev(SDL_GetRenderTarget(renderer))
    {
        SDL_SetRenderTarget(r, target);
    }
    ~RenderTargetGuard() { SDL_SetRenderTarget(r, prev); }
};


// Back-buffer / screen wrapper — renders to the default target (nullptr texture).
GraphicsBuffer::GraphicsBuffer(SDL_Renderer* pRenderer,
    unsigned int width, unsigned int height)
    : mpRenderer(pRenderer)
    , mWidth(width)
    , mHeight(height)
    , mOwns(false)
    , mpTexture(nullptr)   // nullptr == render to screen
{
}

// Load from image file
GraphicsBuffer::GraphicsBuffer(const std::string& filename)
    : mOwns(true)
{
    mpRenderer = sRenderer;
    SDL_Surface* pSurface = IMG_Load(filename.c_str());
    if (!pSurface)
    {
        std::cerr << "IMG_Load error (" << filename << "): " << SDL_GetError() << "\n";
        return;
    }
    mWidth = (unsigned int)pSurface->w;
    mHeight = (unsigned int)pSurface->h;
    mpTexture = SDL_CreateTextureFromSurface(mpRenderer, pSurface);
    SDL_DestroySurface(pSurface);
    if (!mpTexture)
        std::cerr << "SDL_CreateTextureFromSurface error: " << SDL_GetError() << "\n";
}

// Create a blank off-screen render-target texture
GraphicsBuffer::GraphicsBuffer(unsigned int width, unsigned int height, Color color) : mOwns(true), mWidth(width), mHeight(height)
{
    mpRenderer = sRenderer;
    mpTexture = SDL_CreateTexture(mpRenderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        (int)width, (int)height);
    if (!mpTexture)
    {
        std::cerr << "SDL_CreateTexture error: " << SDL_GetError() << "\n";
        return;
    }
    setToColor(color);
}

GraphicsBuffer::~GraphicsBuffer()
{
    if (mOwns && mpTexture)
    {
        SDL_DestroyTexture(mpTexture);
        mpTexture = nullptr;
    }
}

GraphicsBuffer* GraphicsBuffer::clone() const
{
    GraphicsBuffer* pNew = new GraphicsBuffer(mWidth, mHeight);
    pNew->draw(Vector2D(0, 0), *this, 1.0);
    return pNew;
}

void GraphicsBuffer::draw(const Vector2D& destLoc,
    const GraphicsBuffer& src, double scale)
{
    // Never copy a texture onto itself
    assert(mpTexture != src.mpTexture);
    if (mpTexture == src.mpTexture) return;

    RenderTargetGuard guard(mpRenderer, mpTexture);

    SDL_FRect dest;
    dest.x = destLoc.getX();
    dest.y = destLoc.getY();
    dest.w = (float)((double)src.mWidth * scale);
    dest.h = (float)((double)src.mHeight * scale);

    SDL_RenderTexture(mpRenderer, src.mpTexture, nullptr, &dest);
}

void GraphicsBuffer::draw(const Vector2D& destLoc,
    const Sprite& sprite, double scale)
{
    GraphicsBuffer* pSrc = sprite.getBuffer();
    assert(mpTexture != pSrc->mpTexture);
    if (mpTexture == pSrc->mpTexture) return;

    RenderTargetGuard guard(mpRenderer, mpTexture);

    SDL_FRect src;
    src.x = sprite.getSourceLoc().getX();
    src.y = sprite.getSourceLoc().getY();
    src.w = (float)sprite.getWidth();
    src.h = (float)sprite.getHeight();

    SDL_FRect dest;
    dest.x = destLoc.getX();
    dest.y = destLoc.getY();
    dest.w = (float)((double)sprite.getWidth() * scale);
    dest.h = (float)((double)sprite.getHeight() * scale);

    // Alpha blend so sprites composite correctly over the background
    SDL_SetTextureBlendMode(pSrc->mpTexture, SDL_BLENDMODE_BLEND);
    SDL_RenderTexture(mpRenderer, pSrc->mpTexture, &src, &dest);
}

void GraphicsBuffer::writeText(const Vector2D& destLoc,
    const Font& font,
    const Color& color,
    const std::string& text,
    Font::Alignment align)
{
    if (!font.mpFont || text.empty()) return;

    SDL_Color sdlColor = Color::getSDLColorFromColor(color);

    SDL_Surface* pSurface = TTF_RenderText_Blended(font.mpFont,
        text.c_str(), 0, sdlColor);
    if (!pSurface) return;

    SDL_Texture* pTex = SDL_CreateTextureFromSurface(mpRenderer, pSurface);
    SDL_DestroySurface(pSurface);
    if (!pTex) return;

    float textW = 0.0f, textH = 0.0f;
    SDL_GetTextureSize(pTex, &textW, &textH);

    float x = destLoc.getX();
    if (align == Font::CENTER) x -= textW / 2.0f;
    else if (align == Font::RIGHT)  x -= textW;

    SDL_FRect dest{ x, destLoc.getY(), textW, textH };

    {
        RenderTargetGuard guard(mpRenderer, mpTexture);
        SDL_RenderTexture(mpRenderer, pTex, nullptr, &dest);
    }

    SDL_DestroyTexture(pTex);
}


void GraphicsBuffer::setToColor(const Color& color)
{
    RenderTargetGuard guard(mpRenderer, mpTexture);
    SDL_SetRenderDrawColor(mpRenderer, (Uint8)color.getR(), (Uint8)color.getG(), (Uint8)color.getB(), (Uint8)color.getA());
    SDL_RenderClear(mpRenderer);
}

void GraphicsBuffer::saveToFile(const std::string& filename) const
{
    // Redirect rendering to this texture so we can read its pixels
    SDL_SetRenderTarget(mpRenderer, mpTexture);

    // SDL3: SDL_RenderReadPixels(renderer) returns a new SDL_Surface
    SDL_Surface* pSurface = SDL_RenderReadPixels(mpRenderer, nullptr);
    SDL_SetRenderTarget(mpRenderer, nullptr);

    if (!pSurface)
    {
        std::cerr << "saveToFile: SDL_RenderReadPixels failed: "
            << SDL_GetError() << "\n";
        return;
    }

    if (!IMG_SavePNG(pSurface, filename.c_str()))
        std::cerr << "saveToFile: IMG_SavePNG failed: " << SDL_GetError() << "\n";

    SDL_DestroySurface(pSurface);
}




