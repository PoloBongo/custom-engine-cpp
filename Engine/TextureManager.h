#pragma once
#include <cassert>

class TextureManager
{
public:
	// la fonction static va être créer lors du première appel de la fonction
	static RenderManager& get() {
		static RenderManager* gpSingleton = NULL;
		if (gpSingleton == NULL) {
			gpSingleton = new RenderManager;
		}
		assert(gpSingleton);
		return *gpSingleton;
	}
private:
	TextureManager() {}

	~TextureManager() {}

	void start() {}

	void stop() {}
};

