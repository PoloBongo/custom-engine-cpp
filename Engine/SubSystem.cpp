#include "SubSystem.h"

CollisionManager gCollisionManager; // global variable.

void startUp()
{
	gCollisionManager.start();
}