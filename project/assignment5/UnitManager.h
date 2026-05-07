#pragma once
#include <Trackable.h>
#include <vector>
#include "Vector2D.h"

class Unit;
class Animation;

typedef std::string UnitKey;

class UnitManager : public Trackable
{
public:
	UnitManager(unsigned int sceneWidth, unsigned int sceneHeight) : mSceneWidth(sceneWidth), mSceneHeight(sceneHeight) {};
	~UnitManager() { cleanup(); };

	void init() {};
	void cleanup();

	Unit* createAndAddUnit(std::vector<Animation> animationVector, Vector2D pos, float moveSpeed);
	void deleteUnit(Unit* deleteUnit);
	Unit* getUnitAt(Vector2D location);
	Unit* getRecentUnit();
	unsigned int getNumUnits() { return mpUnits.size(); };
	void toggleUnitsFreeze();

	void update(double dt);
	//void draw();
	void draw(Vector2D cameraOffset);


private:
	unsigned int mSceneWidth = 0;
	unsigned int mSceneHeight = 0;
	std::vector<Unit*> mpUnits;
};
