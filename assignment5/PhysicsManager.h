#pragma once
#include <map>
#include "Trackable.h"
#include "Vector2D.h"
#include <vector>

using namespace std;

typedef std::string ColliderKey;

class Collider;
class Object;

class PhysicsManager : public Trackable
{
public:
	struct Collision
	{
		ColliderKey box1, box2;
		Vector2D resolution;
	};
	static PhysicsManager* createInstance();
	inline static PhysicsManager* getInstance() { return mpsInstance; };
	static void destroyInstance();

	void init() {};
	void cleanup();

	Collider* createCollider(const ColliderKey& key, float length, float width = 0, Vector2D center = ZERO_VECTOR2D, bool isTrigger = false, Object* parent = nullptr);
	void deleteCollider(const ColliderKey& key);

	Collider* getCollider(const ColliderKey& key);

	void handleCollisions();
private:
	static PhysicsManager* mpsInstance;

	Collision checkCollision(Collider* object1, Collider* object2);
	void updateColliders();

	PhysicsManager() {};
	inline ~PhysicsManager() { cleanup(); };

	map<ColliderKey, Collider*> mColliderMap;
};
