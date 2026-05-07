#include "PhysicsManager.h"
#include "Collider.h"
#include "EventSystem.h"
#include "CollisionEvent.h"

PhysicsManager* PhysicsManager::mpsInstance = nullptr;

PhysicsManager* PhysicsManager::createInstance()
{
	if (mpsInstance == nullptr)
		mpsInstance = new PhysicsManager();
	return mpsInstance;
}
void PhysicsManager::destroyInstance()
{
	if (mpsInstance)
	{
		delete mpsInstance;
		mpsInstance = nullptr;
	}
}



void PhysicsManager::cleanup()
{
	for (auto iter : mColliderMap)
	{
		Collider* pCollider = iter.second;
		delete pCollider;
	}

	mColliderMap.clear();
}

Collider* PhysicsManager::createCollider(const ColliderKey& key, float length, float width, Vector2D center, bool isTrigger, Object* parent)
{
	Collider* pCollider = nullptr;

	auto iter = mColliderMap.find(key);

	if (iter == mColliderMap.end())
	{
		pCollider = new Collider(key, length, width, center, isTrigger, parent);
		mColliderMap[key] = pCollider;
	}

	return pCollider;
}

void PhysicsManager::deleteCollider(const ColliderKey& key)
{
	auto iter = mColliderMap.find(key);

	if (iter != mColliderMap.end())
	{
		delete iter->second;
		mColliderMap.erase(iter);
	}
}

Collider* PhysicsManager::getCollider(const ColliderKey& key)
{
	auto iter = mColliderMap.find(key);

	if (iter != mColliderMap.end())
	{
		return iter->second;
	}
	//if none found
	return nullptr;
}

void PhysicsManager::updateColliders()
{
	for (unsigned int i = 0; i < mColliderMap.size(); i++)
	{
		next(mColliderMap.begin(), i)->second->update();
	}
}

void PhysicsManager::handleCollisions()
{
	updateColliders();
	for (unsigned int i = 0; i < mColliderMap.size(); i++)
		for (unsigned int j = i + 1; j < mColliderMap.size(); j++)
		{
			Collider* object1 = std::next(mColliderMap.begin(), i)->second;
			Collider* object2 = std::next(mColliderMap.begin(), j)->second;

			// Skip barrier vs barrier static objects never need collision response
			bool obj1IsBarrier = (object1->getTag().find("Barrier") != std::string::npos);
			bool obj2IsBarrier = (object2->getTag().find("Barrier") != std::string::npos);
			if (obj1IsBarrier && obj2IsBarrier) continue;

			Collision results = checkCollision(object1, object2);
			if (results.box1 != "")
			{
				//cout << "Collision Detected: " << results.box1 << " and " << results.box2 << endl;
				EventSystem::getInstance()->fireEvent(CollisionEvent(results));
			}
		}

}

PhysicsManager::Collision PhysicsManager::checkCollision(Collider* object1, Collider* object2)
{
	float separation = 0;
	Vector2D test = ZERO_VECTOR2D;
	struct Collision collisionData;

	separation = test.getDistanceBetween(object1->getCenter(), object2->getCenter()); // distance between centers

	float xSeparation = abs((object1->getCenter().getX() - object2->getCenter().getX())) - (object1->getLength() * 0.5) - (object2->getLength() * 0.5);

	float ySeparation = abs((object1->getCenter().getY() - object2->getCenter().getY())) - (object1->getWidth() * 0.5) - (object2->getWidth() * 0.5);

	if (xSeparation <= 0 && ySeparation <= 0)
	{
		//collided
		
		float halfW0 = object1->getLength() * 0.5f;  
		float halfH0 = object1->getWidth() * 0.5f;  
		float halfW1 = object2->getLength() * 0.5f;
		float halfH1 = object2->getWidth() * 0.5f;

		float dx = object2->getCenter().getX() - object1->getCenter().getX();

		float dy = object2->getCenter().getY() - object1->getCenter().getY();

		float overlapX = (halfW0 + halfW1) - std::abs(dx);

		float overlapY = (halfH0 + halfH1) - std::abs(dy);

		float xRes = 0.0f;

		float yRes = 0.0f;

		if (overlapX < overlapY)
		{
			xRes = (dx > 0) ? overlapX : -overlapX;
		}
		else
		{
			yRes = (dy > 0) ? overlapY : -overlapY;
		}

		collisionData.box1 = object1->getTag();

		collisionData.box2 = object2->getTag();

		if (object1->isTrigger() || object2->isTrigger())
			collisionData.resolution = ZERO_VECTOR2D;
		else
			collisionData.resolution = Vector2D(xRes, yRes)/*.getNormalizedVector()*/;

		return collisionData;
	}

	//no collision
	collisionData.box1 = "";
	return collisionData;
}