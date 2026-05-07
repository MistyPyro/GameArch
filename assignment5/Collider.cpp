#include "Collider.h"

Collider::Collider(string tag, float length, float width, Vector2D center, bool isTrigger, Object* parent)
	:mTag(tag),
	mpParent(parent),
	mLength(length),
	mCenter(center),
	mIsTrigger(isTrigger)
{
	if (width != 0)
		mWidth = width;
	else
		mWidth = length;
}

void Collider::update()
{
	if (mpParent)
	{
		setCenter(mpParent->getPosition());
	}
}