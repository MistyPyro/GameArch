#pragma once
#include <Trackable.h>
#include <SDL3/SDL.h>

class GraphicsBuffer;

class Color : public Trackable
{
    friend class GraphicsBuffer;
public:
    Color() {};
    Color(int r, int g, int b, int a = 255);
    Color(float r, float g, float b, float a = 1.0f);

    int getR() const { return mRed; };
    int getG() const { return mGreen; };
    int getB() const { return mBlue; };
    int getA() const { return mAlpha; };
    float getRf() const { return mRed / 255.0f; };
    float getGf() const { return mGreen / 255.0f; };
    float getBf() const { return mBlue / 255.0f; };
    float getAf() const { return mAlpha / 255.0f; };

private:
    int mRed = 0, mGreen = 0, mBlue = 0, mAlpha = 255;

    static SDL_Color getSDLColorFromColor(const Color& color);
};