#include "Color.h"

Color::Color(int r, int g, int b, int a)
{
    mRed = r;
    mGreen = g;
    mBlue = b;
    mAlpha = a;
}

Color::Color(float r, float g, float b, float a)
{
    mRed = (int)(r * 255);
    mGreen = (int)(g * 255);
    mBlue = (int)(b * 255);
    mAlpha = (int)(a * 255);
}

SDL_Color Color::getSDLColorFromColor(const Color& color)
{
    SDL_Color c;
    c.r = (Uint8)color.mRed;
    c.g = (Uint8)color.mGreen;
    c.b = (Uint8)color.mBlue;
    c.a = (Uint8)color.mAlpha;
    return c;
}
