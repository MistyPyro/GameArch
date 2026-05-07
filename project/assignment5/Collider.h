#pragma once
#include "Trackable.h"
#include "Vector2D.h"
#include "Object.h"
#include <string>
using namespace std;

class Collider : public Trackable
{
public:
	Collider(string tag, float length, float width = 0, Vector2D center = ZERO_VECTOR2D, bool isTrigger = false, Object* parent = nullptr);
	~Collider() {};

	void update();

	inline string getTag() { return mTag; };
	inline float getLength() { return mLength; };
	inline float getWidth() { return mWidth; };
	inline Vector2D getCenter() { return mCenter; };
	inline bool isTrigger() { return mIsTrigger; };

	inline void setCenter(Vector2D newPos) { mCenter = newPos; };
	inline void setCenter(float x, float y) { mCenter = Vector2D(x, y); };
	inline void setLength(float newLength) { mLength = newLength; };
	inline void setWidth(float newWidth) { mWidth = newWidth; };
	inline void setTag(string newTag) { mTag = newTag; };


private:
	Object* mpParent = nullptr;

	string mTag;
	float mLength, mWidth;
	Vector2D mCenter;
	bool mIsTrigger;
};