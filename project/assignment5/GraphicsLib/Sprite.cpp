#include "Sprite.h"

//implement me
Sprite::Sprite(GraphicsBuffer* pBuffer, const Vector2D& ulCorner, unsigned int width, unsigned int height)
{
	mpBuffer = pBuffer;
	mSourceLoc = ulCorner;
	mWidth = width;
	mHeight = height;
}

//implement me
Sprite::Sprite()
{
}

//implement me
Sprite::~Sprite()
{
}
