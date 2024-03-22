#include "SubSystem.h"

CollisionManager* gCollisionManager; // global variable
RenderManager* gRendererManager; // global variable

void startUp()
{
	// Collision Manager
	gCollisionManager = new CollisionManager();
	gCollisionManager->start();
	// Render Manager
	gRendererManager = new RenderManager();
	gRendererManager->start();
}