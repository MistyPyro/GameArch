#pragma once

#include <Trackable.h>
#include <Vector2D.h>
#include <vector>
#include "Animation.h"

class Game;
class Animation;
class GraphicsBuffer;

class Unit : public Trackable
{
	friend class UnitManager;
	friend class PlayerManager;
public:
	enum Borders
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	};

	void update(double dt, Vector2D dir = Vector2D(-2, -2));
	void setPosition(float x, float y) { mLocation = Vector2D(x, y); };
	void setPosition(Vector2D newPos) { mLocation = newPos; };
	void setFrozen(bool isFrozen) { mIsFrozen = isFrozen; };
	void toggleFrozen();
	void setAnimation(Animation pNewAnimation) { mpCurrentAnimation = &pNewAnimation; };
	void addAnimation(Animation pNewAnimation) { mpAnimations.push_back(pNewAnimation); };
	void switchAnimations();
	float millisecondsToSecond(float milliseconds) { return milliseconds / 1000; };
	Vector2D getPosition() { return mLocation; };
	Animation* getCurrentAnimation() { return mpCurrentAnimation; };
	void addDirectionVector(const Vector2D dir) { mDir += dir; };
	void setDirectionVector(const Vector2D dir) { mDir = dir; };
	void draw(Vector2D cameraOffset = Vector2D(0, 0));
	Vector2D getDirectionVector() const { return mDir; }
private:
	Unit(std::vector<Animation> animationVector, unsigned int width, unsigned int height, float moveSpeed);
	~Unit();

	float mMoveSpeed;
	Vector2D mLocation;
	Vector2D mDir;
	Animation* mpCurrentAnimation;
	std::vector<Animation> mpAnimations;
	unsigned int mCurrentAnimationIndex;
	unsigned int mSceneWidth;
	unsigned int mSceneHeight;
	bool mIsFrozen;
};
