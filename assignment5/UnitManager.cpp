#include "UnitManager.h"
#include "Unit.h"
#include "Animation.h"
#include "Sprite.h"
#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"

void UnitManager::cleanup()
{
	for (unsigned int i = 0; i < mpUnits.size(); i++)
		delete mpUnits[i];
	mpUnits.clear();
}

Unit* UnitManager::createAndAddUnit(std::vector<Animation> animationVector,
	Vector2D pos, float moveSpeed)
{
	Unit* pUnit = new Unit(animationVector, mSceneWidth, mSceneHeight, moveSpeed);
	pUnit->setPosition(pos);
	mpUnits.push_back(pUnit);
	return pUnit;
}

void UnitManager::deleteUnit(Unit* deleteUnit)
{
	for (unsigned int i = 0; i < mpUnits.size(); i++)
	{
		if (mpUnits[i] == deleteUnit)
		{
			delete mpUnits[i];
			mpUnits.erase(mpUnits.begin() + i);
			return; 
		}
	}
}

Unit* UnitManager::getUnitAt(Vector2D location)
{
	for (unsigned int i = 0; i < mpUnits.size(); i++)
	{
		Unit* pUnit = mpUnits[i];
		float left = pUnit->mLocation.getX();
		float right = left + pUnit->getCurrentAnimation()->getCurrentSprite().getWidth();
		float top = pUnit->mLocation.getY();
		float bottom = top + pUnit->getCurrentAnimation()->getCurrentSprite().getHeight();

		if (location.getX() > left && location.getX() < right &&
			location.getY() > top && location.getY() < bottom)
			return pUnit;
	}
	return nullptr;
}

Unit* UnitManager::getRecentUnit()
{
	if (!mpUnits.empty())
		return mpUnits.back();
	return nullptr;
}

void UnitManager::toggleUnitsFreeze()
{
	for (unsigned int i = 0; i < mpUnits.size(); i++)
		mpUnits[i]->toggleFrozen();
}

void UnitManager::update(double dt)
{
	// Iterate backwards so erasing doesn't skip the next element
	for (int i = (int)mpUnits.size() - 1; i >= 0; i--)
	{
		mpUnits[i]->update(dt);

		auto* pAnim = mpUnits[i]->getCurrentAnimation();
		unsigned int sw = pAnim->getCurrentSprite().getWidth();
		unsigned int sh = pAnim->getCurrentSprite().getHeight();
		Vector2D     pos = mpUnits[i]->getPosition();

		// Cull units that have left the visible scene area
		if (pos.getX() - (float)sw > (float)mSceneWidth || pos.getX() + (float)sw < 0.0f || pos.getY() - (float)sh >(float)mSceneHeight || pos.getY() + (float)sh < 0.0f)
		{
			// Delete directly here its already going backwards so no index skip
			delete mpUnits[i];
			mpUnits.erase(mpUnits.begin() + i);
		}
	}
}

void UnitManager::draw(Vector2D cameraOffset)
{
	for (unsigned int i = 0; i < mpUnits.size(); i++)
		mpUnits[i]->draw(cameraOffset);
}


