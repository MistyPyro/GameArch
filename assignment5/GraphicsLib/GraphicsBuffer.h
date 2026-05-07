#pragma once
#include <Trackable.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <string>
#include "Color.h"
#include "Font.h"

class GraphicsSystem;
class Vector2D;
class Sprite;

class GraphicsBuffer : public Trackable
{
public:
    GraphicsBuffer(const std::string& filename);
    GraphicsBuffer(unsigned int width, unsigned int height, Color color = Color());
    ~GraphicsBuffer();
    GraphicsBuffer(SDL_Renderer* pRenderer, unsigned int width, unsigned int height);

    GraphicsBuffer* clone() const;

    unsigned int getWidth()  const { return mWidth; };
    unsigned int getHeight() const { return mHeight; };

    void draw(const Vector2D& destLoc, const GraphicsBuffer& src, double scale = 1.0);
    void draw(const Vector2D& destLoc, const Sprite& sprite, double scale = 1.0);
    void writeText(const Vector2D& destLoc, const Font& font, const Color& color,
        const std::string& text, Font::Alignment align = Font::LEFT);
    void setToColor(const Color& color);
    void saveToFile(const std::string& filename) const;

    SDL_Texture* getTexture() const { return mpTexture; }

    static SDL_Renderer* getRenderer() { return sRenderer; }
private:
    SDL_Texture* mpTexture = nullptr;
    SDL_Renderer* mpRenderer = nullptr; 
    bool mOwns = true;
    unsigned int mWidth = 0;
    unsigned int mHeight = 0;

    static SDL_Renderer* sRenderer;
    friend class GraphicsSystem;

    GraphicsBuffer(const GraphicsBuffer&) = delete;
    GraphicsBuffer& operator=(const GraphicsBuffer&) = delete;
};