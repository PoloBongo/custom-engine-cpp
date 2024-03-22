#pragma once
#include <cassert>

class CollisionManager
{
public:
	static CollisionManager& get() {
		static CollisionManager* gpSingleton = NULL;
		if (gpSingleton == NULL) {
			gpSingleton = new CollisionManager;
		}
		assert(gpSingleton);
		return *gpSingleton;
	}

	CollisionManager()
	{
		// nothing
	}

	~CollisionManager()
	{
		// nothing
	}

	void start() {}

	void stop() {}
};