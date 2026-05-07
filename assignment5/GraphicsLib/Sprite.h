#pragma once

#include <Trackable.h>
#include <Vector2D.h>

class GraphicsBuffer;

class Sprite : public Trackable
{
public:
	Sprite(GraphicsBuffer* pBuffer, const Vector2D& ulCorner, unsigned int width, unsigned int height);
	Sprite();//default constructor
	~Sprite();//implement this destructor

	GraphicsBuffer* getBuffer() const { return mpBuffer; };
	const Vector2D& getSourceLoc() const { return mSourceLoc; };
	unsigned int getWidth() const { return mWidth; };
	unsigned int getHeight() const { return mHeight; };
private:
	GraphicsBuffer* mpBuffer;
	unsigned int mHeight = 0, mWidth = 0;
	Vector2D mSourceLoc;
};