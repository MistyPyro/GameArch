#pragma once
#include <Trackable.h>
#include <SDL3/SDL_ttf.h>
#include <string>

class GraphicsBuffer;

class Font : public Trackable
{
    friend class GraphicsBuffer;
public:
    enum Alignment
    {
        CENTER,
        LEFT,
        RIGHT
    };

    Font(const std::string& filename, int size);
    ~Font();
    int getSize() const { return mSize; };

private:
    TTF_Font* mpFont = nullptr;
    int mSize = 0;
};