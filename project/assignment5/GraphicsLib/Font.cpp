#include "Font.h"
#include <cassert>
#include <iostream>

Font::Font(const std::string& filename, int size)
{
    mpFont = TTF_OpenFont(filename.c_str(), (float)size);
    if (!mpFont)
        std::cerr << "TTF_OpenFont error: " << SDL_GetError() << "\n";
    mSize = size;
}

Font::~Font()
{
    if (mpFont)
    {
        std::cout << "FONT DESTRUCTOR: " << mpFont << std::endl;
        TTF_CloseFont(mpFont);
        mpFont = nullptr;
    }
}
