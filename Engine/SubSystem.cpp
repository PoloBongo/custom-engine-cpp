#include "SubSystem.h"

CollisionManager gCollisionManager; // global variable
RenderManager gRendererManager; // global variable

void startUp()
{
	gCollisionManager.start();
	gRendererManager.start();
}