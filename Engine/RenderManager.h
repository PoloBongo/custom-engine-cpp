#pragma once
#include "TextureManager.h"

class RenderManager
{
public:
	RenderManager()
	{
		TextureManager::get();
	}

	~RenderManager() {}

	void start() {}

	void stop() {}
};

